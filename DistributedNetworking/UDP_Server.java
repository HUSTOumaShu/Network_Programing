import java.io.*;
import java.net.*;

public class UDP_Server {
    public static void main(String[] args) {
        DatagramSocket socket = null;
        socket = new DatagramSocket(6789);
        byte[] buffer = new byte[1000];
        while(true) {
            DatagramPacket request = new DatagramPacket(buffer, buffer.length());
            socket.receive(request);
            DatagramPacket reply = new DatagramPacket(request.getData(),
            request.getLength(), request.getAddress(), request.getPort());
            socket.send(reply);
        }
    }
}
