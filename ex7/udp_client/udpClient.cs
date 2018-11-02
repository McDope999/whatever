using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;

class UDPClient
{
  int PORT {get; set;}
  IPAddress serverAddress {get; set;}
  string Command {get; set;}

    private UDPClient (string[] args)
    {

      //If entered set Porbroadcastt and IPAddress
      if(args.Length < 3)
      {
        Console.WriteLine("Too few params: UDPClient 10.0.0.1 U");
      }
      else
      {
        PORT = int.Parse(args[0]);
        serverAddress = IPAddress.Parse(args[1]);
        Command = args[2];
      }

      //  Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Dgram,
      //      ProtocolType.Udp);

        byte[] sendbuf = Encoding.ASCII.GetBytes(Command);
        IPEndPoint ep = new IPEndPoint(serverAddress, PORT);

        UdpClient udpClient = new UdpClient();
        udpClient.Send(sendbuf, sendbuf.Length, ep);

        Console.WriteLine($"Request {Command} sent to server at {serverAddress}, {PORT}");

        byte[] receivedData = udpClient.Receive(ref ep);

        String receivedLine = Encoding.ASCII.GetString(receivedData);
        Console.WriteLine("The message received:");
        Console.WriteLine(receivedLine);

    }

    public static void Main (string[] args)
    {
      new UDPClient(args);
    }

}
