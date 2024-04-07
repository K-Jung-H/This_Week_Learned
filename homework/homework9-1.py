from tkinter import *

class MainGUI:
    width = 200
    height = 100
    ball_x, ball_y = 10, 10
    ball_size = 10

    def up(self):
        if self.ball_y > 10:
            self.canvas.move('ball',0, -5)
            self.ball_y -=5

    def down(self):
        if self.ball_y < 90:
            self.canvas.move('ball', 0, 5)
            self.ball_y +=5

    def left(self):
        if self.ball_x > 10:
            self.canvas.move('ball',-5, 0)
            self.ball_x -=5

    def right(self):
        if self.ball_x < 190:
            self.canvas.move('ball', 5, 0)
            self.ball_x +=5

    def __init__(self):
        window = Tk()
        window.title("공 옮기기")
        width = self.width
        height = self.height
        self.canvas = Canvas(window, bg='white', width=width, height=height)
        self.canvas.pack()
        self.canvas.create_oval(self.ball_x, self.ball_y, self.ball_x+self.ball_size, self.ball_y+self.ball_size, fill='red', tags='ball') #x1,y1,x2,y2
        frame = Frame(window)
        frame.pack()
        Button(frame, text='상', command=self.up).pack(side=LEFT)
        Button(frame, text='하', command=self.down).pack(side=LEFT)
        Button(frame, text='좌', command=self.left).pack(side=LEFT)
        Button(frame, text='우', command=self.right).pack(side=LEFT)
        window.mainloop()

MainGUI()