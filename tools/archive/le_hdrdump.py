import struct
data = open("inputs/SYNDICAT_MAIN.EXE","rb").read()
H = struct.unpack_from("<I", data, 0x3c)[0]
u32 = lambda o: struct.unpack_from("<I", data, H+o)[0]
names = {
 0x14:"n_pages", 0x18:"eip_obj", 0x1c:"eip", 0x28:"page_size", 0x2c:"last_page_bytes",
 0x30:"fixup_sect_size", 0x38:"loader_sect_size",
 0x40:"obj_table_off", 0x44:"n_objects", 0x48:"obj_page_map_off",
 0x4c:"iter_data_map_off", 0x50:"res_table_off", 0x58:"resident_name_off",
 0x5c:"entry_table_off", 0x60:"module_directives_off",
 0x64:"fixup_page_table_off", 0x68:"fixup_record_table_off",
 0x6c:"imp_mod_name_off", 0x70:"n_imp_mods", 0x74:"imp_proc_name_off",
 0x78:"per_page_cksum_off", 0x80:"data_pages_off", 0x84:"preload_pages",
}
print(f"H(e_lfanew)={H:#x}")
for o in sorted(names):
    v = u32(o)
    print(f"  +{o:#04x} {names[o]:<24} = {v:#x}  (abs {v+H:#x})" if o>=0x40 and 'off' in names[o]
          else f"  +{o:#04x} {names[o]:<24} = {v:#x}")
