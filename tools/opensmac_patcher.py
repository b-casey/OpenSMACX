#!/usr/local/bin/python3

# v1.0 : initial

# built-in libraries
import argparse
import mmap
import os
import shutil
import struct
import subprocess
# use pip to install
import pefile

count = 0 # functions patched counter

def patch_call_bytes(file):
    file.write_byte(0xFF)
    file.write_byte(0x25)
    global count
    count += 1

# pass path to exe to patch, dll and folder to write patched exe plus dll to
parser = argparse.ArgumentParser(description="Redirect Functions Patcher")
parser.add_argument("-e",    "--exe", help="Input exe", required=True)
parser.add_argument("-d",    "--dll", help="Input dll", required=True)
parser.add_argument("-o", "--output", help="Output folder path to write exe + dll to", required=True)
args = parser.parse_args()
exe_path = os.path.normpath(args.output) + "\\terranx_opensmacx.exe"

# copying exe+dll to SMACX directory
shutil.copy2(args.exe, exe_path)
shutil.copy2(args.dll, args.output)

print("Adding imports + copying binary...")
argsIA = ("ImportAdder.exe", exe_path)
popen = subprocess.Popen(argsIA, stdout=subprocess.PIPE)
popen.wait()
print("Imports added successfully...")

print("Getting address of first import...")
addr = 0
pe = pefile.PE(exe_path)
for entry in pe.DIRECTORY_ENTRY_IMPORT:
    dll_name = entry.dll.decode('utf-8')
    if dll_name == "OpenSMACX.dll":
        addr = entry.imports[0].address
print("Address of first import found: 0x%08X" % addr)


print("Patching: ", exe_path)
with open(exe_path, "r+b") as f:
    bin_app = mmap.mmap(f.fileno(), 0)
    # next: 448
    #

    # ALPHA
    bin_app.seek(0x00184D60) # ?tech_name@@YAHPAD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*104))
    bin_app.seek(0x00184E40) # ?chas_name@@YAHPAD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*105))
    bin_app.seek(0x00184F40) # ?weap_name@@YAHPAD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*106))
    bin_app.seek(0x00185030) # ?arm_name@@YAHPAD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*107))
    bin_app.seek(0x00185150) # ?tech_item@@YAHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*108))
    bin_app.seek(0x00185170) # ?read_basic_rules@@YAHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*109))
    bin_app.seek(0x00185E30) # ?read_tech@@YAHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*110))
    bin_app.seek(0x00185FE0) # ?clear_faction@@YAXPAUPlayer@@@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*111))
    bin_app.seek(0x00186050) # ?read_faction@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*112))
    bin_app.seek(0x00186090) # ?read_faction@@YAXPAUPlayer@@H@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*113))
    bin_app.seek(0x00186F30) # ?read_factions@@YAHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*114))
    bin_app.seek(0x001871D0) # ?noun_item@@YAXPAIPAH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*115))
    bin_app.seek(0x00187240) # ?read_units@@YAHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*116))
    bin_app.seek(0x001873C0) # ?read_rules@@YAHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*117))
    bin_app.seek(0x0019DBD0) # ?prefs_fac_load@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*118))
    bin_app.seek(0x00216A00) # ?labels_init@@YAHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*119))
    bin_app.seek(0x002169D0) # ?labels_shutdown@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*120))
    bin_app.seek(0x001A5880) # ?say_label@@YAXH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*291))
    bin_app.seek(0x0019DB40) # ?prefs_get@@YAHPBDHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*137))
    bin_app.seek(0x0019D980) # ?prefs_get@@YAPADPBD0H@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*138))
    bin_app.seek(0x0019E510) # ?prefs_put@@YAXPBD0@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*139))
    bin_app.seek(0x0019E530) # ?prefs_put@@YAXPBDHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*140))
    bin_app.seek(0x0019DA20) # ?default_prefs@@YAIXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*383))
    bin_app.seek(0x0019DAA0) # ?default_prefs2@@YAIXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*384))
    bin_app.seek(0x0019DB20) # ?default_warn@@YAIXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*385))    
    bin_app.seek(0x0019DB30) # ?default_rules@@YAIXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*386))
    bin_app.seek(0x0019DCF0) # ?prefs_load@@YAXH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*387))
    bin_app.seek(0x0019E5D0) # ?prefs_save@@YAXH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*388))
    bin_app.seek(0x0019E950) # ?prefs_use@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*389))    
    bin_app.seek(0x00227100) # ?set_language@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*390))
    # BASE
    bin_app.seek(0x000E3B80) # ?base_find@@YAHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*169))
    bin_app.seek(0x000E3C60) # ?base_find@@YAHHHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*244))
    bin_app.seek(0x000E3D50) # ?base_find@@YAHHHHHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*245))    
    bin_app.seek(0x000F80D0) # ?has_project@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*170))
    bin_app.seek(0x00100290) # ?has_fac_built@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*171))
    bin_app.seek(0x001002E0) # ?base_project@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*172))
    bin_app.seek(0x0010BA00) # ?bitmask@@YAXIPAI0@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*173))
    bin_app.seek(0x001BF010) # ?is_sensor@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*253))
    bin_app.seek(0x000E3A00) # ?say_base@@YAXPADH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*257))
    bin_app.seek(0x000E39D0) # ?set_base@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*258))
    bin_app.seek(0x00179A00) # ?is_port@@YAHIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*260))
    bin_app.seek(0x000EF090) # ?pop_goal@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*261))
    bin_app.seek(0x000EEF80) # ?pop_goal_fac@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*262))
    bin_app.seek(0x001BA0E0) # ?facility_avail@@YAHIIHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*268))
    bin_app.seek(0x000E48B0) # ?set_fac@@YAXIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*269))
    bin_app.seek(0x00021670) # ?has_fac@@YAHHIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*270))
    bin_app.seek(0x001BA030) # ?redundant@@YAHHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*271))
    bin_app.seek(0x000E4810) # ?base_lose_minerals@@YAHIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*281))
    bin_app.seek(0x000E4700) # ?base_making@@YAHHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*282))
    bin_app.seek(0x000E4020) # ?best_specialist@@YAIXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*286))
    bin_app.seek(0x000E4090) # ?name_base@@YAXIPADHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*305))
    bin_app.seek(0x001AC680) # ?ascending@@YAHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*330))
    bin_app.seek(0x001AC630) # ?transcending@@YAHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*331))
    bin_app.seek(0x001AC060) # ?is_objective@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*334))
    bin_app.seek(0x000F6510) # ?fac_maint@@YAIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*335))
    bin_app.seek(0x000E4900) # ?has_fac_announced@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*338))
    bin_app.seek(0x000E4960) # ?set_fac_announced@@YAXIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*339))
    bin_app.seek(0x00160B30) # ?garrison_check@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*365))
    bin_app.seek(0x00160D30) # ?defensive_check@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*366))
    bin_app.seek(0x0019E980) # ?vulnerable@@YAHIHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*368))
    bin_app.seek(0x000EA1F0) # ?black_market@@YAIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*370))
    bin_app.seek(0x000F4DC0) # ?base_energy_costs@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*373))
    bin_app.seek(0x000E4AA0) # ?base_first@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*374))
    bin_app.seek(0x000E6400) # ?morale_mod@@YAIIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*375))
    bin_app.seek(0x000E65C0) # ?breed_mod@@YAIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*376))
    bin_app.seek(0x000E6740) # ?worm_mod@@YAIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*377))
    bin_app.seek(0x000E9B70) # ?base_nutrient@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*378))    
    bin_app.seek(0x000E9CB0) # ?base_minerals@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*379))
    bin_app.seek(0x000E4430) # ?cost_factor@@YAHIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*441))
    bin_app.seek(0x000F7FE0) # ?make_base_unique@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*442))
    # BASEBUTTON
    bin_app.seek(0x00207550) # ?set_bubble_text@BaseButton@@QAEHPBD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*242))
    bin_app.seek(0x002074E0) # ?set_name@BaseButton@@QAEHPBD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*243))
    # COUNCIL
    bin_app.seek(0x0012AE20) # ?eligible@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*325))
    bin_app.seek(0x0012AD30) # ?council_votes@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*326))    
    # FACTION
    bin_app.seek(0x001B4730) # ?society_avail@@YAHHHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*264))
    bin_app.seek(0x0010B910) # ?get_adjective@@YAPADI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*289))
    bin_app.seek(0x0010B930) # ?get_noun@@YAPADI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*290))
    bin_app.seek(0x00139E40) # ?climactic_battle@@YAHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*332))
    bin_app.seek(0x00139D40) # ?aah_ooga@@YAIHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*333))
    bin_app.seek(0x001A96D0) # ?see_map_check@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*340))
    bin_app.seek(0x00160D50) # ?guard_check@@YAIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*364))
    bin_app.seek(0x0019EE50) # ?corner_market@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*369))
    bin_app.seek(0x00179A30) # ?add_goal@@YAXIHHHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*401))
    bin_app.seek(0x00179B70) # ?add_site@@YAXIHHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*402))
    bin_app.seek(0x00179CC0) # ?at_goal@@YAHIHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*403))
    bin_app.seek(0x00179D20) # ?at_site@@YAHIHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*404))
    bin_app.seek(0x00179D80) # ?wipe_goals@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*405))
    bin_app.seek(0x00179E00) # ?init_goals@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*406))
    bin_app.seek(0x00179E70) # ?del_site@@YAXIHHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*407))
    bin_app.seek(0x00139160) # ?auto_contact@@YAHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*418))
    bin_app.seek(0x001002F0) # ?has_treaty@@YAIIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*423))
    bin_app.seek(0x0013A030) # ?cause_friction@@YAXIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*431))
    bin_app.seek(0x0013A090) # ?get_mood@@YAIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*432))
    bin_app.seek(0x0013A100) # ?reputation@@YAIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*433))
    bin_app.seek(0x0013A150) # ?get_patience@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*434))
    bin_app.seek(0x0013A1C0) # ?energy_value@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*435))
    bin_app.seek(0x0015BB30) # ?set_treaty@@YAXIIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*436))
    bin_app.seek(0x0015BBA0) # ?set_agenda@@YAXIIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*437))
    bin_app.seek(0x00139B70) # ?great_beelzebub@@YAHIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*438))
    bin_app.seek(0x00139C00) # ?great_satan@@YAHIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*439))
    bin_app.seek(0x001B0D70) # ?compute_faction_modifiers@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*443))
    bin_app.seek(0x00139EF0) # ?at_climax@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*444))
    bin_app.seek(0x001B4210) # ?social_calc@@YAXPAUSocialCategory@@PAUSocialEffect@@IHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*445))
    bin_app.seek(0x001B44D0) # ?social_upkeep@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*446))
    bin_app.seek(0x001B4550) # ?social_upheaval@@YAIIPAUSocialCategory@@@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*447))    
    # FILEMAP
    bin_app.seek(0x00228380) # ??0Filemap@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*60))
    bin_app.seek(0x002283E0) # ??1Filemap@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*61))
    bin_app.seek(0x002283A0) # ?init@Filemap@@QAEPAV1@PBDH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*62))
    bin_app.seek(0x002283C0) # ?init@Filemap@@QAEPAV1@PBD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*63))
    bin_app.seek(0x00228430) # ?open_read@Filemap@@QAEPAXPBDH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*64))
    bin_app.seek(0x00228540) # ?open@Filemap@@QAEPAXPBDH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*65))
    bin_app.seek(0x00228650) # ?create@Filemap@@QAEPAXPBDIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*66))
    bin_app.seek(0x002287C0) # ?close@Filemap@@QAEXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*67))
    bin_app.seek(0x00228810) # ?close@Filemap@@QAEXPAX@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*68))    
    # FONT
    bin_app.seek(0x00218EA0) # ??0Font@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*80))
    bin_app.seek(0x00218EC0) # ??0Font@@QAE@PADHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*81))
    bin_app.seek(0x00218EE0) # ??1Font@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*82))
    bin_app.seek(0x00218F40) # ?init@Font@@QAEHPBDHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*83))
    bin_app.seek(0x002190D0) # ?init@Font@@QAEHPBD0HI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*84))
    bin_app.seek(0x00219230) # ?close@Font@@QAEXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*85))
    bin_app.seek(0x00219280) # ?width@Font@@QAEHPAD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*86))
    bin_app.seek(0x002192F0) # ?width@Font@@QAEHPADI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*87))
    bin_app.seek(0x00219370) # ?find_line_break_l@Font@@QAEPADPADPAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*88))
    bin_app.seek(0x002195B0) # ?init_font_class@Font@@SAHPAV1@@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*89))
    bin_app.seek(0x00219610) # ?close_font_class@Font@@SAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*90))
    # GAME
    bin_app.seek(0x001C89A0) # ?game_year@@YAIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*279))
    bin_app.seek(0x001C89B0) # ?say_year@@YAXPAD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*280))
    bin_app.seek(0x001B38D0) # ?clear_scenario@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*324))
    bin_app.seek(0x00189180) # ?planetfall@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*420))
    # GENERAL
    bin_app.seek(0x00200780) # ?purge_trailing@@YAXPAD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*121))
    bin_app.seek(0x00200760) # ?purge_leading@@YAXPAD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*122))
    bin_app.seek(0x002007B0) # ?purge_spaces@@YAXPAD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*123))
    bin_app.seek(0x00200820) # ?kill_lf@@YAXPAD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*124))
    bin_app.seek(0x00200840) # ?add_lf@@YAXPAD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*125))
    bin_app.seek(0x00022F00) # ?range@@YAHHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*126))
    bin_app.seek(0x001D4510) # ?mem_get_old@@YAPAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*127))
    bin_app.seek(0x00234BB0) # ?env_open_old@@YAPAU_iobuf@@PBD0@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*128))
    bin_app.seek(0x001A58E0) # ?parse_set@@YAXHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*247))    
    bin_app.seek(0x00225E30) # ?parse_num@@YAHIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*129))
    bin_app.seek(0x00225E50) # ?parse_say@@YAHIHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*130))
    bin_app.seek(0x00225EC0) # ?parse_says@@YAHIPBDHH@Z    
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*131))
    bin_app.seek(0x002288D0) # ?btoi@@YAHPBD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*132))
    bin_app.seek(0x002288F0) # ?htoi@@YAHPBD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*133))
    bin_app.seek(0x00228950) # ?stoi@@YAHPBD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*134))
    bin_app.seek(0x00228B30) # ?findnum@@YAPADPAD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*135))
    bin_app.seek(0x0022D570) # ?jackal_version_check@@YAHPBD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*136))
    bin_app.seek(0x002003A0) # ?filefind_cd_drive_letter@@YADXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*141))
    bin_app.seek(0x002003B0) # ?filefind_set_alternative@@YAXPBD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*142))
    bin_app.seek(0x002005D0) # ?filefind_get@@YAPADPBD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*143))
    bin_app.seek(0x0010BA30) # ?bit_count@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*144))
    bin_app.seek(0x00228AB0) # ?bit_count_signed@@YAIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*272))
    bin_app.seek(0x00138FB0) # ?my_srand@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*273))
    bin_app.seek(0x00228A50) # ?swap@@YAXPAH0@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*274))
    bin_app.seek(0x00228A80) # ?swap@@YAXPAE0@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*275))
    bin_app.seek(0x00228AD0) # ?fixed_div@@YAHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*276))
    bin_app.seek(0x00228AF0) # ?memrchr@@YAPBDPBD0D@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*277))
    bin_app.seek(0x002290E0) # ?quick_root@@YAHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*278))
    bin_app.seek(0x00139090) # ?checksum@@YAEPAEIE@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*341))
    bin_app.seek(0x001390C0) # ?checksum_password@@YAIPBD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*342))
    bin_app.seek(0x00179770) # ?rnd@@YAIHPAD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*348))
    bin_app.seek(0x00138F30) # ?danger@@YAXPBD0HHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*380))
    bin_app.seek(0x001ABD10) # ?kill_auto_save@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*413))
    bin_app.seek(0x001ABD20) # ?auto_save@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*414))
    bin_app.seek(0x001ABE40) # ?load_undo@@YAXH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*415))
    bin_app.seek(0x001ABEC0) # ?wipe_undo@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*416))
    bin_app.seek(0x001ABF20) # ?auto_undo@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*417))
    bin_app.seek(0x0017D1F0) # ?header_check@@YAXPADPAU_iobuf@@@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*421))
    bin_app.seek(0x0017D240) # ?header_write@@YAXPBDPAU_iobuf@@@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*422))
    # HEAP
    bin_app.seek(0x001D4560) # ??0Heap@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*0))
    bin_app.seek(0x001D4580) # ??1Heap@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*1))
    bin_app.seek(0x001D45B0) # ?shutdown@Heap@@QAEXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*2))
    bin_app.seek(0x001D45E0) # ?squeeze@Heap@@QAEXH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*3))
    bin_app.seek(0x001D4620) # ?init@Heap@@QAEHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*4))
    bin_app.seek(0x001D4680) # ?get@Heap@@QAEPAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*5))
    # LOG
    bin_app.seek(0x00225FB0) # ??0Log@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*45))
    bin_app.seek(0x00225FC0) # ??0Log@@QAE@PBD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*46))
    bin_app.seek(0x00226020) # ??1Log@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*47))
    bin_app.seek(0x00226040) # ?init@Log@@QAEHPBD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*48))
    bin_app.seek(0x002260D0) # ?reset@Log@@QAEXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*49))
    bin_app.seek(0x002260F0) # ?say@Log@@QAEXPBD0HHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*50))
    bin_app.seek(0x00226190) # ?say_hex@Log@@QAEXPBD0HHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*51))
    bin_app.seek(0x00225F20) # ?log_logging@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*52))
    bin_app.seek(0x00225F90) # ?log_logging_exit@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*53))
    bin_app.seek(0x00226230) # ?log_reset@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*54))
    bin_app.seek(0x00226250) # ?log_say@@YAXPBD0HHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*55))
    bin_app.seek(0x002262F0) # ?log_say@@YAXPBDHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*56))
    bin_app.seek(0x00226350) # ?log_say_hex@@YAXPBD0HHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*57))
    bin_app.seek(0x002263F0) # ?log_say_hex@@YAXPBDHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*58))
    bin_app.seek(0x00226450) # ?log_set_state@@YAXH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*59))    
    # MAP
    bin_app.seek(0x000712A0) # ?on_map@@YAHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*252))    
    bin_app.seek(0x00179790) # ?x_dist@@YAHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*145))
    bin_app.seek(0x00191AD0) # ?temp_set@@YAXIIE@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*146))
    bin_app.seek(0x00191A80) # ?climate_set@@YAXIIE@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*147))
    bin_app.seek(0x00100150) # ?alt_at@@YAIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*148))
    bin_app.seek(0x00100180) # ?alt_detail_at@@YAIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*149))
    bin_app.seek(0x00191260) # ?alt_put_detail@@YAXIIE@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*150))
    bin_app.seek(0x00191B10) # ?owner_set@@YAXIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*151))
    bin_app.seek(0x00191B50) # ?site_set@@YAXIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*152))
    bin_app.seek(0x00100220) # ?region_at@@YAIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*153))
    bin_app.seek(0x00191B90) # ?region_set@@YAXIIE@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*154))
    bin_app.seek(0x00191C10) # ?using_set@@YAXIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*155))
    bin_app.seek(0x00191C50) # ?lock_set@@YAXIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*156))
    bin_app.seek(0x00191C90) # ?lock_map@@YAHIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*157))
    bin_app.seek(0x00191CF0) # ?unlock_map@@YAXIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*158))
    bin_app.seek(0x00191BC0) # ?rocky_set@@YAXIIE@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*159))
    bin_app.seek(0x001001B0) # ?bit_at@@YAIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*160))
    bin_app.seek(0x00191D30) # ?bit_put@@YAXIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*161))
    bin_app.seek(0x00191D60) # ?bit_set@@YAXIIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*162))
    bin_app.seek(0x00191DB0) # ?bit2_set@@YAXIIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*163))
    bin_app.seek(0x00191E00) # ?code_set@@YAXIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*164))
    bin_app.seek(0x00191E50) # ?synch_bit@@YAXIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*165))
    bin_app.seek(0x001001E0) # ?is_ocean@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*166))
    bin_app.seek(0x00100250) # ?veh_who@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*167))
    bin_app.seek(0x00132A90) # ?rebuild_vehicle_bits@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*201))
    bin_app.seek(0x00132B70) # ?rebuild_base_bits@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*246))
    bin_app.seek(0x001798E0) # ?anything_at@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*168))
    bin_app.seek(0x000E49D0) # ?is_coast@@YAHHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*259))
    bin_app.seek(0x0008BEE0) # ?xrange@@YAHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*283))
    bin_app.seek(0x000F8090) # ?vector_dist@@YAHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*284))
    bin_app.seek(0x001C89F0) # ?zoc_any@@YAIHHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*297))
    bin_app.seek(0x001C8AC0) # ?zoc_veh@@YAIHHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*298))
    bin_app.seek(0x001C8BA0) # ?zoc_sea@@YAIHHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*299))
    bin_app.seek(0x001C8D40) # ?zoc_move@@YAIIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*300))
    bin_app.seek(0x000E3EF0) # ?whose_territory@@YAHIIIPAHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*309))
    bin_app.seek(0x000E3FA0) # ?base_territory@@YAHIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*310))
    bin_app.seek(0x001BF130) # ?has_temple@@YAHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*311))
    bin_app.seek(0x00191F00) # ?minerals_at@@YAIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*312))
    bin_app.seek(0x00192030) # ?bonus_at@@YAIIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*313))
    bin_app.seek(0x00192140) # ?goody_at@@YAIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*314))
    bin_app.seek(0x00190ED0) # ?map_init@@YAHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*315))
    bin_app.seek(0x00191130) # ?map_read@@YAHPAU_iobuf@@@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*316))
    bin_app.seek(0x00190E90) # ?map_shutdown@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*317))    
    bin_app.seek(0x00191040) # ?map_wipe@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*318))
    bin_app.seek(0x001910B0) # ?map_write@@YAHPAU_iobuf@@@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*319))
    bin_app.seek(0x00191210) # ?abstract_at@@YAEII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*320))
    bin_app.seek(0x00191230) # ?abstract_set@@YAXIIE@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*321))
    bin_app.seek(0x001591C0) # ?bad_reg@@YAHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*329))
    bin_app.seek(0x001919C0) # ?elev_at@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*344))
    bin_app.seek(0x001918A0) # ?alt_natural@@YAIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*345))
    bin_app.seek(0x00179840) # ?is_known@@YAHIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*346))    
    bin_app.seek(0x001798A0) # ?base_who@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*347))
    bin_app.seek(0x0010DDC0) # ?sea_coast@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*349))
    bin_app.seek(0x0010DE00) # ?sea_coasts@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*350))
    bin_app.seek(0x0010DE50) # ?base_on_sea@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*351))
    bin_app.seek(0x0010DF30) # ?base_coast@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*352))
    bin_app.seek(0x0010E030) # ?port_to_coast@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*353))
    bin_app.seek(0x0010E160) # ?port_to_port@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*354))
    bin_app.seek(0x0010E310) # ?transport_base@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*355))
    bin_app.seek(0x0010E3C0) # ?naval_base@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*356))
    bin_app.seek(0x0010E5C0) # ?convoy@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*357))
    bin_app.seek(0x0016B320) # ?get_there@@YAHIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*358))    
    bin_app.seek(0x0016B480) # ?coast_or_border@@YAHIIIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*359))
    bin_app.seek(0x000ECB90) # ?crappy@@YAIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*308))
    bin_app.seek(0x001A5910) # ?vector_dist@@YAHHHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*372))
    bin_app.seek(0x00192400) # ?site_radius@@YAXHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*396))
    bin_app.seek(0x00193830) # ?quick_zoc@@YAXIIIHHPAH0@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*397))
    bin_app.seek(0x001A65A0) # ?radius_move@@YAHHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*398))
    bin_app.seek(0x001A65D0) # ?radius_move@@YAHHHHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*399))
    bin_app.seek(0x001A6630) # ?compass_move@@YAHHHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*400))
    # PATH
    bin_app.seek(0x0019A220) # ?init@Path@@QAEXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*322))
    bin_app.seek(0x0019A2D0) # ?shutdown@Path@@QAEXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*323))
    bin_app.seek(0x0019A370) # ?zoc_path@Path@@QAEHIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*408))
    bin_app.seek(0x0019C200) # ?make_abstract@Path@@QAEXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*409))
    bin_app.seek(0x0019C340) # ?merge@Path@@QAEXII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*410))
    bin_app.seek(0x0019C3C0) # ?territory@Path@@QAEXIIHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*411))
    bin_app.seek(0x0019C520) # ?continent@Path@@QAEXIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*412))
    bin_app.seek(0x0019C790) # ?continents@Path@@QAEXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*419))    
    # PROBE
    bin_app.seek(0x0010C4B0) # ?steal_energy@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*343))
    bin_app.seek(0x0019EA80) # ?mind_control@@YAHIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*371))
    bin_app.seek(0x0019EEE0) # ?success_rates@@YAHIIHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*382))    
    # RANDOM
    bin_app.seek(0x00225730) # ??0Random@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*69))
    bin_app.seek(0x00225740) # ??1Random@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*70))
    bin_app.seek(0x00225750) # ?reseed@Random@@QAEXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*71))
    bin_app.seek(0x00225770) # ?get@Random@@QAEIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*72))
    bin_app.seek(0x002257B0) # ?get@Random@@QAENXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*73))
    bin_app.seek(0x00225700) # ?random_rand@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*74))
    bin_app.seek(0x00225720) # ?random_rand_exit@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*75))
    bin_app.seek(0x002257E0) # ?random_reseed@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*76))
    bin_app.seek(0x00225800) # ?random_get@@YAIXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*77))
    bin_app.seek(0x00225810) # ?random@@YAIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*78))
    bin_app.seek(0x00225850) # ?random@@YANXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*79))
    # SPOT
    bin_app.seek(0x001FA860) # ??0Spot@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*91))
    bin_app.seek(0x001FA870) # ??1Spot@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*92))
    bin_app.seek(0x001FA820) # ?clear@Spot@@QAEXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*93))
    bin_app.seek(0x001FA830) # ?shutdown@Spot@@QAEXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*94))
    bin_app.seek(0x001FA8A0) # ?init@Spot@@QAEXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*95))
    bin_app.seek(0x001FA900) # ?replace@Spot@@QAEXHHHHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*96))
    bin_app.seek(0x001FA960) # ?add@Spot@@QAEHHHHHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*97))
    bin_app.seek(0x001FA9C0) # ?kill_pos@Spot@@QAEXH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*98))
    bin_app.seek(0x001FAA10) # ?kill_specific@Spot@@QAEXHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*99))
    bin_app.seek(0x001FAA90) # ?kill_type@Spot@@QAEXH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*100))
    bin_app.seek(0x001FAB00) # ?check@Spot@@QAEHHHPAH0@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*101))
    bin_app.seek(0x001FAB70) # ?check@Spot@@QAEHHHPAH0PAUtagRECT@@@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*102))
    bin_app.seek(0x001FAC00) # ?get_rect@Spot@@QAEHHHPAUtagRECT@@@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*103))    
    # STRINGS
    bin_app.seek(0x002168D0) # ??0Strings@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*6))
    bin_app.seek(0x002169C0) # ??1Strings@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*7))
    bin_app.seek(0x002168F0) # ?init@Strings@@QAEHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*8))
    bin_app.seek(0x00216950) # ?shutdown@Strings@@QAEXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*9))
    bin_app.seek(0x00216970) # ?put@Strings@@QAEPADPBD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*10))
    bin_app.seek(0x002169A0) # ?get@Strings@@QAEPADH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*11))
    # TECHNOLOGY
    bin_app.seek(0x001B9F20) # ?has_tech@@YAHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*174))
    bin_app.seek(0x001BDD70) # ?tech_mil@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*175))
    bin_app.seek(0x001BDD90) # ?tech_tech@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*176))
    bin_app.seek(0x001BDDC0) # ?tech_infra@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*177))
    bin_app.seek(0x001BDDF0) # ?tech_colonize@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*178))
    bin_app.seek(0x001BAC20) # ?tech_avail@@YAHIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*265))
    bin_app.seek(0x001B9FE0) # ?tech_category@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*292))
    bin_app.seek(0x001B9F90) # ?tech_recurse@@YAHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*293))
    bin_app.seek(0x001B9EF0) # ?tech_name@@YAPADHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*294))
    bin_app.seek(0x001591A0) # ?say_tech@@YAXHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*295))
    bin_app.seek(0x001B9C40) # ?say_tech@@YAXPADHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*296))
    bin_app.seek(0x001BCB60) # ?tech_is_preq@@YAHHHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*327))
    bin_app.seek(0x001BAE60) # ?tech_effects@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*328))
    bin_app.seek(0x001BDC10) # ?tech_ai@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*337))
    bin_app.seek(0x001BCBE0) # ?tech_val@@YAHIHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*360))
    bin_app.seek(0x0017CE50) # ?valid_tech_leap@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*361))
    bin_app.seek(0x001BE6B0) # ?tech_rate@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*362))    
    # TERRAFORMING
    bin_app.seek(0x000C9A50) # ?contribution@@YAIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*198))    
    bin_app.seek(0x001BAB40) # ?terrain_avail@@YAHIHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*266))
    bin_app.seek(0x000C9420) # ?terraform_cost@@YAIHHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*381))
    # TEXT
    bin_app.seek(0x001FD860) # ??0Text@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*12))
    bin_app.seek(0x001FD880) # ??0Text@@QAE@I@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*13))
    bin_app.seek(0x00208C00) # ??1Text@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*14))
    bin_app.seek(0x001FD8D0) # ?init@Text@@QAEHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*15))
    bin_app.seek(0x001FD970) # ?shutdown@Text@@QAEXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*16))
    bin_app.seek(0x001FD9D0) # ?close@Text@@QAEXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*17))
    bin_app.seek(0x001FDA00) # ?open@Text@@QAEHPBD0@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*18))
    bin_app.seek(0x001FDC10) # ?get@Text@@QAEPADXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*19))
    bin_app.seek(0x001FDC80) # ?string@Text@@QAEPADXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*20))
    bin_app.seek(0x001FDD10) # ?item@Text@@QAEPADXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*21))
    bin_app.seek(0x001FDD80) # ?item_string@Text@@QAEPADXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*22))
    bin_app.seek(0x001FDDF0) # ?item_number@Text@@QAEHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*23))
    bin_app.seek(0x001FDE60) # ?item_binary@Text@@QAEHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*24))
    bin_app.seek(0x001FDED0) # ?item_hex@Text@@QAEHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*25))
    bin_app.seek(0x001FD400) # ?text_txt@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*26))
    bin_app.seek(0x001FD460) # ?text_txt_exit@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*27))
    bin_app.seek(0x001FD530) # ?text_close@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*28))
    bin_app.seek(0x001FD550) # ?text_open@@YAHPBD0@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*29))
    bin_app.seek(0x001FD570) # ?text_get@@YAPADXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*30))
    bin_app.seek(0x001FD5E0) # ?text_string@@YAPADXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*31))
    bin_app.seek(0x001FD670) # ?text_item@@YAPADXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*32))
    bin_app.seek(0x001FD6D0) # ?text_item_string@@YAPADXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*33))
    bin_app.seek(0x001FD740) # ?text_item_number@@YAHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*34))
    bin_app.seek(0x001FD7A0) # ?text_item_binary@@YAHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*35))
    bin_app.seek(0x001FD800) # ?text_item_hex@@YAHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*36))
    bin_app.seek(0x00185120) # ?text_get_number@@YAHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*37))
    bin_app.seek(0x001FD4C0) # ?text_set_get_ptr@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*287))
    bin_app.seek(0x001FD4D0) # ?text_set_item_ptr@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*288))
    # TEXTINDEX
    bin_app.seek(0x001FDF40) # ??0TextIndex@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*38))
    bin_app.seek(0x001FDF60) # ??1TextIndex@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*39))
    bin_app.seek(0x001FDF80) # ?make_index@TextIndex@@QAEXPBD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*40))
    bin_app.seek(0x001FE120) # ?search_index@TextIndex@@QAEHPBD0@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*41))
    bin_app.seek(0x001FE1F0) # ?text_make_index@@YAXPBD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*42))
    bin_app.seek(0x001FE230) # ?text_search_index@@YAHPBD0@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*43))
    bin_app.seek(0x001FE270) # ?text_clear_index@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*44))
    # TIME
    bin_app.seek(0x002161D0) # ??0Time@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*179))
    bin_app.seek(0x00216200) # ??1Time@@QAE@XZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*180))
    bin_app.seek(0x00216260) # ?init@Time@@QAEXP6AXH@ZHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*181))
    bin_app.seek(0x002162D0) # ?init@Time@@QAEXP6AXHH@ZHHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*182))
    bin_app.seek(0x00216350) # ?start@Time@@QAEHP6AXH@ZHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*183))
    bin_app.seek(0x00216410) # ?start@Time@@QAEHP6AXHH@ZHHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*184))
    bin_app.seek(0x002164D0) # ?pulse@Time@@QAEHP6AXH@ZHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*185))
    bin_app.seek(0x00216590) # ?pulse@Time@@QAEHP6AXHH@ZHHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*186))
    bin_app.seek(0x00216650) # ?start@Time@@QAEHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*187))
    bin_app.seek(0x002166C0) # ?pulse@Time@@QAEHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*188))
    bin_app.seek(0x00216730) # ?stop@Time@@QAEXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*189))
    bin_app.seek(0x00216780) # ?close@Time@@QAEXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*190))
    bin_app.seek(0x00216860) # ?set_modal@Time@@QAEXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*191))
    bin_app.seek(0x00216870) # ?release_modal@Time@@QAEXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*192))
    bin_app.seek(0x002167E0) # ?TimerProc@Time@@SAXPAUHWND__@@III@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*193))
    bin_app.seek(0x00216820) # ?MultimediaProc@Time@@SAXIIIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*194))
    bin_app.seek(0x00216880) # ?init_class@Time@@SAHXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*195))
    bin_app.seek(0x00216890) # ?close_class@Time@@SAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*196))
    bin_app.seek(0x001FD370) # ?flush_timer@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*197))
    bin_app.seek(0x0010F3D0) # ?start_timers@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*306))
    bin_app.seek(0x0010F440) # ?stop_timers@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*307))    
    # VEH
    bin_app.seek(0x00100320) # ?drop_range@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*199))
    bin_app.seek(0x00101500) # ?psi_factor@@YAHHIHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*200))
    bin_app.seek(0x001004F0) # ?planet_buster2@@YAIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*255))
    bin_app.seek(0x00100520) # ?planet_buster@@YAIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*256))
    bin_app.seek(0x00179920) # ?veh_top@@YAHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*202))
    bin_app.seek(0x00179960) # ?veh_moves@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*203))
    bin_app.seek(0x001799A0) # ?proto_power@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*204))
    bin_app.seek(0x0017D270) # ?arm_strat@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*205))
    bin_app.seek(0x0017D2E0) # ?weap_strat@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*206))
    bin_app.seek(0x0017D360) # ?weap_val@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*207))
    bin_app.seek(0x0017D3F0) # ?arm_val@@YAHIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*208))
    bin_app.seek(0x0017D480) # ?armor_val@@YAHIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*209))
    bin_app.seek(0x0017D510) # ?transport_val@@YAIIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*210))
    bin_app.seek(0x0017D560) # ?say_offense@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@I@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*211))
    bin_app.seek(0x0017D6D0) # ?say_defense@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@I@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*212))
    bin_app.seek(0x0017D7D0) # ?say_stats_3@@YAXPADI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*213))
    bin_app.seek(0x0010B9A0) # ?say_stats_3@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*214))
    bin_app.seek(0x0017D8E0) # ?say_stats_2@@YAXPADI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*215))
    bin_app.seek(0x0017DAA0) # ?say_stats@@YAXPADI0@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*216))
    bin_app.seek(0x0017EFA0) # ?best_reactor@@YAIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*248))
    bin_app.seek(0x0017EFF0) # ?pick_chassis@@YAHHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*249))
    bin_app.seek(0x0017F0B0) # ?weapon_budget@@YAHHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*250))
    bin_app.seek(0x0017F150) # ?armor_budget@@YAHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*251))
    bin_app.seek(0x00181170) # ?abil_index@@YAHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*254))
    bin_app.seek(0x001A59B0) # ?veh_put@@YAXIHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*217))
    bin_app.seek(0x001A59E0) # ?veh_health@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*218))
    bin_app.seek(0x001A5A60) # ?proto_cost@@YAIIIIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*219))
    bin_app.seek(0x001A5D00) # ?base_cost@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*220))
    bin_app.seek(0x001A5D40) # ?make_proto@@YAXHIIIII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*221))
    bin_app.seek(0x001BFE90) # ?veh_at@@YAHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*222))
    bin_app.seek(0x001BF1F0) # ?has_abil@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*223))
    bin_app.seek(0x001BFFA0) # ?veh_lift@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*224))
    bin_app.seek(0x001C0080) # ?veh_drop@@YAHIHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*225))
    bin_app.seek(0x001C01A0) # ?sleep@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*226))
    bin_app.seek(0x001C01D0) # ?veh_demote@@YAXH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*227))
    bin_app.seek(0x001C0260) # ?veh_promote@@YAXH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*228))
    bin_app.seek(0x001C02D0) # ?veh_clear@@YAXIHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*229))
    bin_app.seek(0x001C0DB0) # ?can_arty@@YAHIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*230))
    bin_app.seek(0x001C0E40) # ?morale_veh@@YAIIHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*231))
    bin_app.seek(0x001C1150) # ?offense_proto@@YAIIHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*232))
    bin_app.seek(0x001C1290) # ?armor_proto@@YAIIHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*233))
    bin_app.seek(0x001C13B0) # ?speed_proto@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*234))
    bin_app.seek(0x001C1540) # ?speed@@YAIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*235))
    bin_app.seek(0x001C1760) # ?veh_cargo@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*236))
    bin_app.seek(0x001C17D0) # ?prototype_factor@@YAII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*237))
    bin_app.seek(0x001C1C40) # ?veh_jail@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*238))
    bin_app.seek(0x001C1D20) # ?veh_skip@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*239))
    bin_app.seek(0x001C1D50) # ?veh_fake@@YAHHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*240))
    bin_app.seek(0x001C1D70) # ?veh_wake@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*241))
    bin_app.seek(0x001BA910) # ?veh_avail@@YAHIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*263))
    bin_app.seek(0x00101350) # ?morale_alien@@YAIHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*267))
    bin_app.seek(0x00193510) # ?hex_cost@@YAHHHIIIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*285))
    bin_app.seek(0x001B8AF0) # ?stack_put@@YAXHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*301))
    bin_app.seek(0x001B8B60) # ?stack_sort@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*302))
    bin_app.seek(0x001B8C90) # ?stack_sort_2@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*303))
    bin_app.seek(0x001B8E10) # ?stack_fix@@YAHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*304))
    bin_app.seek(0x00160AD0) # ?go_to@@YAXIEHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*336))
    bin_app.seek(0x001BE100) # ?wants_prototype@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*363))
    bin_app.seek(0x001B9580) # ?stack_check@@YAHHIHHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*367))
    bin_app.seek(0x00106490) # ?invasions@@YAXI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*391))
    bin_app.seek(0x00179F80) # ?want_monolith@@YAHI@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*392))
    bin_app.seek(0x001AED50) # ?get_plan@@YAHII@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*393))
    bin_app.seek(0x001B5EA0) # ?want_to_wake@@YAHIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*394))
    bin_app.seek(0x001B6060) # ?wake_stack@@YAXH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*395))
    bin_app.seek(0x000B3FD0) # ?say_morale@@YAXPADIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*424))
    bin_app.seek(0x000B43C0) # ?say_morale@@YAXIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*425))
    bin_app.seek(0x001015B0) # ?get_basic_offense@@YAHIHIHH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*426))
    bin_app.seek(0x00101940) # ?get_basic_defense@@YAHIHIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*427))
    bin_app.seek(0x001C1850) # ?veh_cost@@YAIIHPAH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*428))
    bin_app.seek(0x00101D30) # ?battle_init@@YAXXZ
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*429))
    bin_app.seek(0x00101D50) # ?add_bat@@YAXIHPBD@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*430))
    bin_app.seek(0x001010C0) # ?defense_value@@YAIIIIIH@Z
    patch_call_bytes(bin_app)
    bin_app.write(struct.pack("<L", addr+4*440))
    #
    print("Functions redirected: %d" % count)
    print("Done!")
