// Enable thorough analysis options. Used when importing the real .LE via the
// LX loader (which already sets the correct object bases, applies fixups, and
// defines the entry point -- so no manual disassembly seeding is needed here).

import ghidra.app.script.GhidraScript;

public class EnableAggressive extends GhidraScript {
    @Override
    public void run() throws Exception {
        setAnalysisOption(currentProgram, "Aggressive Instruction Finder", "true");
        setAnalysisOption(currentProgram, "Decompiler Parameter ID", "true");
    }
}
