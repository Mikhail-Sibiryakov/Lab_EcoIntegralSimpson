/*
 * <character encoding> Cyrillic (UTF-8 with signature) - Codepage 65001 </character encoding>
 * <summary> Event receiver implementation. </summary>
 * <description> Laboratory 3: two sources and two shared event receivers. </description>
 * <author> Copyright (c) 2026 Mikhail Sibiryakov. All rights reserved. </author>
 */

#include "CEcoIntegralSimpsonSink.h"

/* <summary> Write through the Eco file service. </summary>
 * <description> A partial write makes the client fail. </description> */
void Lab3Print(Lab3Output* output, char_t* text) {
    uint32_t size = 0;
    uint32_t written = 0;
    while (text[size] != 0) {
        size++;
    }
    written = size;
    if (output->file->pVTbl->Write(output->file, text, &written) != 0 || written != size) {
        output->status = ERR_ECO_FAIL;
    }
}

/* <summary> Format an unsigned count without libc. </summary>
 * <description> The stack buffer covers uint32_t. </description> */
void Lab3PrintCount(Lab3Output* output, uint32_t value) {
    char_t text[11];
    uint32_t position = 10;
    text[position] = 0;
    do {
        position--;
        text[position] = (char_t)('0' + value % 10);
        value /= 10;
    } while (value != 0);
    Lab3Print(output, &text[position]);
}

/* <summary> Print finite event data in scientific notation. </summary>
 * <description> Ten displayed digits do not change the stored long-double payload. </description> */
void Lab3PrintReal(Lab3Output* output, long double value) {
    char_t text[14];
    uint32_t i = 0;
    uint32_t digit = 0;
    int16_t exponent = 0;
    if (value == 0.0L) {
        Lab3Print(output, "0");
        return;
    }
    if (value < 0.0L) {
        Lab3Print(output, "-");
        value = -value;
    }
    while (value >= 10.0L) {
        value /= 10.0L;
        exponent++;
    }
    while (value < 1.0L) {
        value *= 10.0L;
        exponent--;
    }
    for (i = 0; i < 10; i++) {
        digit = (uint32_t)value;
        if (digit > 9) {
            digit = 9;
        }
        text[i + (i != 0)] = (char_t)('0' + digit);
        value = (value - (long double)digit) * 10.0L;
    }
    text[1] = '.';
    text[11] = 0;
    Lab3Print(output, text);
    Lab3Print(output, "e");
    if (exponent < 0) {
        Lab3Print(output, "-");
        exponent = (int16_t)-exponent;
    }
    else {
        Lab3Print(output, "+");
    }
    Lab3PrintCount(output, (uint32_t)exponent);
}

/* <summary> Expose the receiver identity and event interface. </summary>
 * <description> Both interfaces share one reference count. </description> */
static int16_t ECOCALLMETHOD SinkQuery(IEcoIntegralSimpsonEvents* me, const UGUID* iid, voidptr_t* out) {
    if (out == 0) {
        return ERR_ECO_POINTER;
    }
    *out = 0;
    if (me == 0 || iid == 0) {
        return ERR_ECO_POINTER;
    }
    if (!IsEqualUGUID(iid, &IID_IEcoUnknown) && !IsEqualUGUID(iid, &IID_IEcoIntegralSimpsonEvents)) {
        return ERR_ECO_NOINTERFACE;
    }
    *out = me;
    me->pVTbl->AddRef(me);
    return 0;
}

/* <summary> Retain a receiver for a subscription. </summary>
 * <description> The caller also owns its initial reference. </description> */
static uint32_t ECOCALLMETHOD SinkAddRef(IEcoIntegralSimpsonEvents* me) {
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    return ++((CEcoIntegralSimpsonSink*)me)->references;
}

/* <summary> Release a receiver and its allocator. </summary>
 * <description> Sources are borrowed to avoid a source-receiver reference cycle. </description> */
static uint32_t ECOCALLMETHOD SinkRelease(IEcoIntegralSimpsonEvents* me) {
    CEcoIntegralSimpsonSink* object = (CEcoIntegralSimpsonSink*)me;
    IEcoMemoryAllocator1* memory = 0;
    uint32_t remaining = 0;
    if (me == 0) {
        return (uint32_t)ERR_ECO_POINTER;
    }
    memory = object->memory;
    remaining = --object->references;
    if (remaining == 0) {
        memory->pVTbl->Free(memory, object);
        memory->pVTbl->Release(memory);
    }
    return remaining;
}

/* <summary> Resolve a borrowed source identity. </summary>
 * <description> Unknown sources mark the sequence as invalid. </description> */
static int16_t SourceIndex(CEcoIntegralSimpsonSink* object, IEcoUnknown* source) {
    if (source == object->sources[0]) {
        return 0;
    }
    if (source == object->sources[1]) {
        return 1;
    }
    object->invalidSequence = 1;
    return -1;
}

/* <summary> Prefix each journal record with its source number. </summary>
 * <description> Receiver identity alone does not identify an event source. </description> */
static void Prefix(CEcoIntegralSimpsonSink* object, uint32_t index) {
    Lab3Print(object->output, "source ");
    Lab3PrintCount(object->output, index + 1);
    Lab3Print(object->output, " | ");
}

/* <summary> Receive original bounds and requested tolerances. </summary>
 * <description> The statistics receiver stays silent. </description> */
static int16_t ECOCALLMETHOD Started(IEcoIntegralSimpsonEvents* me, IEcoUnknown* source,
    uint16_t precision, long double a, long double b, long double epsabs, long double epsrel) {
    CEcoIntegralSimpsonSink* object = (CEcoIntegralSimpsonSink*)me;
    int16_t index = -1;
    Lab3Statistics* stats = 0;
    if (me == 0 || source == 0) {
        return ERR_ECO_POINTER;
    }
    index = SourceIndex(object, source);
    if (index < 0) {
        return ERR_ECO_INVALIDARG;
    }
    stats = &object->statistics[index];
    if (stats->running) {
        object->invalidSequence = 1;
    }
    stats->starts++;
    stats->running = 1;
    stats->lastStep = 0;
    stats->precision = precision;
    if (object->journal) {
        Prefix(object, (uint32_t)index);
        Lab3Print(object->output, "start type=");
        Lab3PrintCount(object->output, precision);
        Lab3Print(object->output, " a=");
        Lab3PrintReal(object->output, a);
        Lab3Print(object->output, " b=");
        Lab3PrintReal(object->output, b);
        Lab3Print(object->output, " epsabs=");
        Lab3PrintReal(object->output, epsabs);
        Lab3Print(object->output, " epsrel=");
        Lab3PrintReal(object->output, epsrel);
        Lab3Print(object->output, "\n");
    }
    return 0;
}

/* <summary> Receive a completed split and the new overall approximation. </summary>
 * <description> Check ordering without assuming monotonically decreasing estimates. </description> */
static int16_t ECOCALLMETHOD Refined(IEcoIntegralSimpsonEvents* me, IEcoUnknown* source,
    uint32_t step, uint32_t intervals, long double left, long double middle,
    long double right, long double value, long double error) {
    CEcoIntegralSimpsonSink* object = (CEcoIntegralSimpsonSink*)me;
    int16_t index = -1;
    Lab3Statistics* stats = 0;
    if (me == 0 || source == 0) {
        return ERR_ECO_POINTER;
    }
    index = SourceIndex(object, source);
    if (index < 0) {
        return ERR_ECO_INVALIDARG;
    }
    stats = &object->statistics[index];
    if (!stats->running || step != stats->lastStep + 1 || intervals != step + 1 ||
        !(left < middle && middle < right) || error < 0.0L) {
        object->invalidSequence = 1;
    }
    stats->lastStep = step;
    stats->refinements++;
    if (object->journal) {
        Prefix(object, (uint32_t)index);
        Lab3Print(object->output, "split ");
        Lab3PrintCount(object->output, step);
        Lab3Print(object->output, " [");
        Lab3PrintReal(object->output, left);
        Lab3Print(object->output, ", ");
        Lab3PrintReal(object->output, middle);
        Lab3Print(object->output, ", ");
        Lab3PrintReal(object->output, right);
        Lab3Print(object->output, "] intervals=");
        Lab3PrintCount(object->output, intervals);
        Lab3Print(object->output, " Q=");
        Lab3PrintReal(object->output, value);
        Lab3Print(object->output, " E=");
        Lab3PrintReal(object->output, error);
        Lab3Print(object->output, "\n");
    }
    return 0;
}

/* <summary> Receive the final status and successful result. </summary>
 * <description> Numeric fields are ignored when computation failed. </description> */
static int16_t ECOCALLMETHOD Finished(IEcoIntegralSimpsonEvents* me, IEcoUnknown* source,
    int16_t status, long double value, long double error) {
    CEcoIntegralSimpsonSink* object = (CEcoIntegralSimpsonSink*)me;
    int16_t index = -1;
    Lab3Statistics* stats = 0;
    if (me == 0 || source == 0) {
        return ERR_ECO_POINTER;
    }
    index = SourceIndex(object, source);
    if (index < 0) {
        return ERR_ECO_INVALIDARG;
    }
    stats = &object->statistics[index];
    if (!stats->running) {
        object->invalidSequence = 1;
    }
    stats->running = 0;
    stats->finishes++;
    stats->status = status;
    if (status == 0) {
        stats->value = value;
        stats->error = error;
    }
    if (object->journal) {
        Prefix(object, (uint32_t)index);
        Lab3Print(object->output, "finish status=");
        if (status < 0) {
            Lab3Print(object->output, "-");
            Lab3PrintCount(object->output, (uint32_t)-(int32_t)status);
        }
        else {
            Lab3PrintCount(object->output, (uint32_t)status);
        }
        if (status == 0) {
            Lab3Print(object->output, " Q=");
            Lab3PrintReal(object->output, value);
            Lab3Print(object->output, " E=");
            Lab3PrintReal(object->output, error);
        }
        Lab3Print(object->output, "\n");
    }
    return 0;
}

static IEcoIntegralSimpsonEventsVTbl sinkVTbl = {
    SinkQuery, SinkAddRef, SinkRelease, Started, Refined, Finished
};

/* <summary> Create one journal or statistics receiver. </summary>
 * <description> Only the allocator is retained; the client owns borrowed services. </description> */
int16_t Lab3CreateSink(IEcoMemoryAllocator1* memory, Lab3Output* output,
    IEcoUnknown** sources, uint8_t journal, CEcoIntegralSimpsonSink** result) {
    CEcoIntegralSimpsonSink* object = 0;
    if (result == 0) {
        return ERR_ECO_POINTER;
    }
    *result = 0;
    if (memory == 0 || output == 0 || sources == 0) {
        return ERR_ECO_POINTER;
    }
    object = (CEcoIntegralSimpsonSink*)memory->pVTbl->Alloc(memory, sizeof(CEcoIntegralSimpsonSink));
    if (object == 0) {
        return ERR_ECO_OUTOFMEMORY;
    }
    memory->pVTbl->Fill(memory, object, 0, sizeof(CEcoIntegralSimpsonSink));
    object->pVTbl = &sinkVTbl;
    object->references = 1;
    object->memory = memory;
    memory->pVTbl->AddRef(memory);
    object->sources[0] = sources[0];
    object->sources[1] = sources[1];
    object->output = output;
    object->journal = journal;
    *result = object;
    return 0;
}
