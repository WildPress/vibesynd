import subprocess, re
L=open("tools/_cmp.txt").read().splitlines()
t=[l for l in L if l.startswith("target")][0].split(": ",1)[1].strip()
o=[l for l in L if l.startswith("ours")][0].split(": ",1)[1].strip()
tb=bytes.fromhex(t)
ob=bytes.fromhex(o); i=ob.find(bytes.fromhex("53565755")); oc=ob[i:]
open("tools/_t.bin","wb").write(tb)
open("tools/_o.bin","wb").write(oc)
def dis(fn):
    r=subprocess.run(["objdump","-D","-b","binary","-mi386","-Mintel",fn],capture_output=True,text=True)
    out=[]
    for ln in r.stdout.splitlines():
        m=re.match(r"\s*([0-9a-f]+):\s+((?:[0-9a-f]{2} )+)\s*(.*)",ln)
        if not m: continue
        by=m.group(2).split()
        txt=m.group(3)
        # normalize: zero out ds:0x... and rel targets and imm addrs
        txt=re.sub(r"0x[0-9a-f]+","0x0",txt)
        out.append((int(m.group(1),16),len(by),m.group(3),txt,"".join(by)))
    return out
T=dis("tools/_t.bin"); O=dis("tools/_o.bin")
# align by walking; when mnemonic-normalized differs, print context
print("instr count T=%d O=%d ; bytes T=%d O=%d"%(len(T),len(O),len(tb),len(oc)))
n=min(len(T),len(O))
for k in range(n):
    if T[k][1]!=O[k][1]:
        print("LEN DIFF at #%d T@%s(%dB) O@%s(%dB)"%(k,hex(T[k][0]),T[k][1],hex(O[k][0]),O[k][1]))
        for d in range(-3,4):
            kk=k+d
            a="%s (%dB)"%(T[kk][2],T[kk][1]) if 0<=kk<len(T) else ""
            b="%s (%dB)"%(O[kk][2],O[kk][1]) if 0<=kk<len(O) else ""
            print("   %-34s | %-34s"%(a,b))
        print()
