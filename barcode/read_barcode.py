import sys
#import urllib
import os
import webbrowser
import Tkinter as tk
'''
master = tk.Tk()

e = Entry(master)
e.pack()

e.focus_set()

quitButton = tk.Button(text='Quit',
    command=self.quit)            
quitButton.grid()  

mainloop()


class Application(tk.Frame):              
    def __init__(self, master=None):
        tk.Frame.__init__(self, master)   
        self.grid()                       
        self.createWidgets()
        

    def createWidgets(self):
        self.quitButton = tk.Button(self, text='Quit',
            command=self.quit)            
        self.quitButton.grid()            

app = Application()                       
app.master.title('Sample application')
e = tk.Entry(app.master, width=50)
e.pack()
app.mainloop()            

'''


while 1:
    print 'press c then enter at any time to quit'
    line = sys.stdin.readline().rstrip()
    #f = urllib.urlopen("http://www.google.com/search?%s" % urllib.urlencode( {'q':line} ))
    #print f.read()
    if line=='c':
        sys.exit(1)

    if 'http' in line:
        os.startfile(line)
    else:
        print 'wrong address'

    #webbrowser.open(line[, new=0[, autoraise=True]])

    
