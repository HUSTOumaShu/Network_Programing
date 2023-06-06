import java.io.*;
import java.net.*;

public class Intranet_Client {
    public static void main(String[] args) throws IOException {
        String msg;
        String modifiedMsg;
        int server_port = 0;
        String server_ip = "";
        Socket cSocket = null;
        if (args.length != 2) {
            System.out.println("Parameter is missing!");
            System.exit(0);
        }

        /*
         * Connect to the server
         */
        try {
            server_port = Integer.parseInt(args[1]);
            server_ip = args[0];
        } catch (Exception e) {
            System.out.println("Require: [IP address] [port]");
            System.exit(0);
        }
        try {
            System.out.println("Connecting to TCP Server at [" + server_ip + ":" + server_port + "]");
            cSocket = new Socket(server_ip, server_port);
        } catch (Exception e) {
            System.out.println("Cannot connect to the server! Please check the server and run again!");
        }

        System.out.println("Server connected.Local client prot: " + cSocket.getLocalPort());
        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
        DataOutputStream out = new DataOutputStream(cSocket.getOutputStream());

        BufferedReader inFromServer = new BufferedReader(new BufferedReader(
                new InputStreamReader(cSocket.getInputStream())));
        System.out.println("Enter the message to send to server: ");
        msg = in.readLine();
        out.writeBytes(msg + '\n');
        modifiedMsg = inFromServer.readLine();
        System.out.println("Received from server: " + modifiedMsg);

        cSocket.close();
    }
}
