// Export every function in the current program to functions.json for the decomp
// manifest. Run via Ghidra headless as a -postScript with the output path as arg.
//
//   analyzeHeadless <projLoc> <proj> -import OBJECT1.linear.bin \
//     -processor x86:LE:32:default -loader BinaryLoader -loader-baseAddr 0x0 \
//     -scriptPath reverse/ghidra_scripts -postScript ExportFunctions.java <out.json>
//
// Fields per function match manifest/README.md: addr, name, size, calls (unique
// outgoing callees; 0 == leaf), data_refs (data references from the body),
// incoming (callers). Addresses are relative to base 0x0, i.e. byte offsets into
// the imported segment -- so target bytes can be pulled straight from the .bin.

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressIterator;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceManager;

import java.io.PrintWriter;
import java.util.Set;

public class ExportFunctions extends GhidraScript {

    private static String esc(String s) {
        return s.replace("\\", "\\\\").replace("\"", "\\\"");
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        String outPath = (args.length > 0) ? args[0] : "functions.json";

        ReferenceManager rm = currentProgram.getReferenceManager();
        FunctionIterator fns = currentProgram.getFunctionManager().getFunctions(true);

        PrintWriter pw = new PrintWriter(outPath, "UTF-8");
        pw.println("{");
        pw.println("  \"binary\": \"SYNDICAT_MAIN.EXE\",");
        pw.println("  \"segment\": \"" + esc(currentProgram.getName()) + "\",");
        pw.println("  \"image_base\": \"" + currentProgram.getImageBase().toString() + "\",");
        pw.println("  \"language\": \"" + esc(currentProgram.getLanguageID().getIdAsString()) + "\",");
        pw.println("  \"functions\": [");

        boolean first = true;
        int total = 0;
        while (fns.hasNext() && !monitor.isCancelled()) {
            Function f = fns.next();
            Address entry = f.getEntryPoint();
            long size = f.getBody().getNumAddresses();

            Set<Function> callees = f.getCalledFunctions(monitor);
            Set<Function> callers = f.getCallingFunctions(monitor);

            int dataRefs = 0;
            AddressIterator ait = f.getBody().getAddresses(true);
            while (ait.hasNext()) {
                Address a = ait.next();
                for (Reference ref : rm.getReferencesFrom(a)) {
                    if (ref.getReferenceType().isData()) {
                        dataRefs++;
                    }
                }
            }

            if (!first) pw.println(",");
            first = false;
            pw.print("    {");
            pw.print("\"addr\": \"" + entry.toString() + "\", ");
            pw.print("\"name\": \"" + esc(f.getName()) + "\", ");
            pw.print("\"size\": " + size + ", ");
            pw.print("\"calls\": " + callees.size() + ", ");
            pw.print("\"data_refs\": " + dataRefs + ", ");
            pw.print("\"incoming\": " + callers.size() + ", ");
            pw.print("\"status\": \"unmatched\", \"match_pct\": 0.0, \"src\": null");
            pw.print("}");
            total++;
        }

        pw.println();
        pw.println("  ]");
        pw.println("}");
        pw.close();

        println("ExportFunctions: wrote " + total + " functions to " + outPath);
    }
}
