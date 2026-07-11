// Pre-analysis seed for a raw code segment: disassemble at the image base so
// Ghidra's auto-analysis has an instruction to follow. Flow through calls/jumps
// plus function-start pattern matching then discovers the rest.

import ghidra.app.script.GhidraScript;

public class SeedDisassemble extends GhidraScript {
    @Override
    public void run() throws Exception {
        disassemble(currentProgram.getImageBase());
    }
}
