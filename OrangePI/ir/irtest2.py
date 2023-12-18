import lirc
sockid = lirc.init("myprogram")
while True:
  ircode = lirc.nextcode()
  if ircode != []:
     print ircode