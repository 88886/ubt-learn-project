import socket
s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
host = socket.gethostname()
port = 8888
s.connect(('localhost',port))
print(type(s.recv(1024)))
s.close()