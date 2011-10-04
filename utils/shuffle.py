import random

def shuffle(x):
	for i in reversed(range(1, len(x))):
		j = random.randrange(i+1)
		x[i], x[j] = x[j], x[i]

c = ["TAXDORFF","IBURRIS","ADCAMPBE","BOCH","FDARLING","CDIONY","DUANMUA","TELSHAMY","KFOZNOT","XIRT","TGRUNN","HERTAL","YIFHUANG","CIOANNOU","IRVINC","NKLUGMAN","ALIPNICK","SALUBER","SHAFEEN","HAVENM","FMUHIB","RLNAPIER","KNASTO","ELIZPAR","JROMEOO","SCHULJOH","JOSHSTEP","ADAMWILL","WUBS","WUPB","FRANCISZ","WOLFPHIL"]

shuffle(c)

i = 1
for name in c:
	print i,name
	i+=1
