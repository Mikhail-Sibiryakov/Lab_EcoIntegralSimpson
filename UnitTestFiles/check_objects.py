"""Audit the component and client objects, separately from shipped Eco libraries."""
from pathlib import Path
import subprocess

project = Path(__file__).resolve().parents[1]
directory = project / 'BuildFiles/Linux/x86_64/StaticRelease'
expected = {
    'CEcoIntegralSimpson.o', 'CEcoIntegralSimpsonFactory.o', 'EcoIntegralSimpson.o',
    'CEcoLab2.o', 'CEcoLab2Factory.o', 'EcoLab2.o',
    'Lab2TestRegistration.o',
}
allowed = {
    'GetIEcoComponentFactoryPtr_BA973067BA954E8FA50BDDE55BC4E3DF',
    'g_xCEcoIntegralSimpson_5BC4E3DF',
    'g_xCEcoLab2_47E653D2',
    'GetIEcoComponentFactoryPtr_7C9B7CECBC69410F9B283F6647E653D2',
    'GetIEcoComponentFactoryPtr_4828F6552E4540E78121EBD220DC360E',
    'GetIEcoComponentFactoryPtr_AE202E543CE54550899603BD70C62565',
    'GetIEcoComponentFactoryPtr_A77A8179F52342DCA168E9A2FB42C673',
    'GetIEcoComponentFactoryPtr_3A8E44677E82475CB4A3719ED8397E61',
    'GetIEcoComponentFactoryPtr_872FEF1DE3314B87AD44D1E7C232C2F0',
    'Lab2RegisterCalculators',
}
objects = sorted(directory.glob('*.o'))
if {path.name for path in objects} != expected:
    raise SystemExit('Missing or unexpected own objects; build the complete project first')
transcript = []
unexpected = []
for path in objects:
    result = subprocess.run(['nm', '-u', str(path)], text=True, capture_output=True, check=True)
    symbols = [line.split()[-1] for line in result.stdout.splitlines() if line.strip()]
    transcript.append(str(path.relative_to(project)))
    transcript.extend('  U ' + symbol for symbol in symbols)
    if not symbols:
        transcript.append('  no undefined symbols')
    unexpected.extend((path, symbol) for symbol in symbols if symbol not in allowed)
output = project / 'BuildFiles/verification/object-symbols.txt'
output.parent.mkdir(parents=True, exist_ok=True)
output.write_text('\n'.join(transcript) + '\n')
if unexpected:
    for path, symbol in unexpected:
        print('FAIL', path.relative_to(project), symbol)
    raise SystemExit(1)
print(f'Own object audit: {len(objects)} objects, no external calls to libc')
