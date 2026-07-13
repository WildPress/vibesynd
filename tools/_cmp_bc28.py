import sys
lines=open("tools/_cmp.txt").read().splitlines()
t=[l for l in lines if l.startswith("target")][0].split(": ",1)[1].strip()
o=[l for l in lines if l.startswith("ours")][0].split(": ",1)[1].strip()
tb=bytes.fromhex(t); ob=bytes.fromhex(o)
i=ob.find(bytes.fromhex("53565755"))
oc=ob[i:]
print("target len",len(tb),"ours code len",len(oc),"diff",len(oc)-len(tb))
def rows(b,n=16):
    return [b[k:k+n].hex() for k in range(0,len(b),n)]
tr=rows(tb); orr=rows(oc)
for k in range(max(len(tr),len(orr))):
    tt=tr[k] if k<len(tr) else ""
    oo=orr[k] if k<len(orr) else ""
    if tt==oo:
        print("%04x    T %s"%(k*16,tt))
    else:
        print("%04x << T %s"%(k*16,tt))
        print("        O %s"%oo)
