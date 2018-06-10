300R

It took me a bit of debugging using Virtual II's inspector and a Wozanium ROM image to figure out the commands.
G - new Game
CR(carriage return) - tells computer to move  (other commands do not requite a carriage return)
R - Reverse sides
yxyxM = Move piece from position y,x to position y,x - computer confirms by printing proposed move. Ex: 1323M
L - resign and start over
Q - Quit and return to monitor
P - Print current board
