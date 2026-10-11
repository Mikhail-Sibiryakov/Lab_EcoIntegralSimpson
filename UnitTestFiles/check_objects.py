"""Audit the component and client objects, separately from shipped Eco libraries."""
from pathlib import Path
import subprocess

project = Path(__file__).resolve().parents[1]
directory = project / 'BuildFiles/Linux/x86_64/StaticRelease'
expected = {
    'CEcoIntegralSimpson.o', 'CEcoIntegralSimpsonFactory.o', 'EcoIntegralSimpson.o',
}
allowed = {
    'GetIEcoComponentFactoryPtr_BA973067BA954E8FA50BDDE55BC4E3DF',
    'g_xCEcoIntegralSimpson_5BC4E3DF',
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
