f: ../a.dummy ../c.dummy
	touch f

f: ../b.dummy

e: ../a.dummy
	touch e

a: b d
	touch a

d: e
d: f
	touch d

b:
	touch b
