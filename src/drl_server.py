import socket
import threading

def handle_client(conn, addr):
    print(f'Connected by {addr}')
    with conn:
        while True:
            print('Waiting for data..')
            data = conn.recv(1024)
            if not data:
                break
            print(f'Received: {data.decode()}. Replying.')
            conn.sendall(data)
    print(f'Connection closed by {addr}')

def main():
    HOST = '127.0.0.1'
    PORT = 65432        
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind((HOST, PORT))
    server_socket.listen()

    print(f'Server is listening on port {PORT}...')

    while True:
        conn, addr = server_socket.accept()
        print(f'New connection from {addr}')
        # create a new thread to handle the client connection
        client_thread = threading.Thread(target=handle_client, args=(conn, addr))
        client_thread.start()

if __name__ == '__main__':
    main()
