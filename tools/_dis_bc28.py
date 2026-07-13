import subprocess
lines=open("tools/_cmp.txt").read().splitlines()
o=[l for l in lines if l.startswith("ours")][0].split(": ",1)[1].strip()
ob=bytes.fromhex(o)
i=ob.find(bytes.fromhex("53565755"))
oc=ob[i:]
open("tools/_ours.bin","wb").write(oc)
print("code bytes:",len(oc))
r=subprocess.run(["objdump","-D","-b","binary","-mi386","-Mintel","tools/_ours.bin"],
                 capture_output=True,text=True)
print(r.stdout)
