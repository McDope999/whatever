using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;


public class UDPServer
{

  private int PORT;
  private IPAddress localAddress;
  private UdpClient udpListener;
  private IPEndPoint udpClient;

    private UDPServer(string[] args)
    {
      if(args.Length > 0)
				PORT = int.Parse(args[0]);
      else
      {
          PORT = 9000;
      }
			if(args.Length > 1)
        localAddress = IPAddress.Parse(args[1]);
      else
        localAddress = IPAddress.Parse("10.0.0.1");

      Console.WriteLine("Server started..");


        udpListener = new UdpClient(PORT);
        udpClient = new IPEndPoint(IPAddress.Any,PORT);
  

        byte[] bytes;


        try
        {
            while (true)
            {
                String receivedCommand = receiveClientCommand();
              Console.WriteLine($"Client command received: {receivedCommand}");

              switch(receivedCommand.ToUpper())
                {

                  case "U":

                    Console.WriteLine($"Sending content from /proc/uptime");
                    sendFileContentUDP("/proc/uptime");
                    break;
                  case "L":
                    Console.WriteLine($"Sending content from /proc/loadavg");
                    sendFileContentUDP("/proc/loadavg");
                    break;
                }
            }

        }
        catch (Exception e)
        {
            Console.WriteLine("Exception: {0}", e.ToString());
        }
        finally
        {
            udpListener.Close();
        }
    }

    String receiveClientCommand()
    {
      byte[] bytes = udpListener.Receive( ref udpClient);

      return Encoding.ASCII.GetString(bytes);

    }

    void sendFileContentUDP(string file)
    {
      FileStream fs = new FileStream(file, FileMode.Open);
      StreamReader s = new StreamReader(fs, Encoding.Default);
      string line =  s.ReadLine();
      Console.WriteLine($"Content sent: {line}");
      s.Close();
      fs.Close();


     byte[] sendbuf = Encoding.ASCII.GetBytes(line);

        udpListener.Send(sendbuf, sendbuf.Length, udpClient);
    }

    public static void Main(string[] args)
    {
        new UDPServer(args);

    }


}
