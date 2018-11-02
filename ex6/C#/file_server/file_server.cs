using System;
using System.IO;
using System.Text;
using System.Net;
using System.Net.Sockets;
using tcp;

namespace tcp
{
	public class file_server
	{

		/// The PORT
		int PORT = 9000;
			/// The BUFSIZE
		const int BUFSIZE = 1000;
		Char[] bytes = new Char[BUFSIZE];
		IPAddress localAddress = IPAddress.Parse("10.0.0.1");


		private file_server (string[] args)
		{
			if(args.Length > 0)
				PORT = int.Parse(args[0]);

			if(args.Length > 1)
				localAddress = IPAddress.Parse(args[1]);

			TcpListener server = null;
				server = new TcpListener(localAddress, PORT);
				try{
					while (true)
					{
						server.Start();

		      	// Enter the listening loop.

			      	        System.Console.Write("Waiting for a connection... ");

			        // Perform a blocking call to accept requests.
			        // You could also user server.AcceptSocket() here.
			        TcpClient client = server.AcceptTcpClient();
			        System.Console.WriteLine("Connected!");


			        // Get a stream object for reading and writing
			        NetworkStream SocketStream = client.GetStream();

							string fileName = tcp.LIB.readTextTCP(SocketStream);

							System.Console.WriteLine(fileName);

							long fileSize = tcp.LIB.check_File_Exists(fileName);

							tcp.LIB.writeTextTCP(SocketStream, $"{fileSize}");

							if(fileSize != 0)
							{
								Console.WriteLine($"Sending file..");
								sendFile(fileName, fileSize, SocketStream);
							}
							else
							{
								Console.WriteLine($"File did not exist on sever. Closing connection..");
								SocketStream.Close();
								client.Close();
							}

						}
					}

			catch(SocketException e){
				Console.WriteLine("SocketException: {0}", e);
			}
			finally{
				// Stop listening for new clients.
			 server.Stop();
		 }

			}

		private void sendFile (String fileName, long fileSize, NetworkStream io)
		{
			FileStream fsr = File.OpenRead(fileName);
			long offset = 0;
			long bytesRead;
			while (offset < fileSize)
					{
						byte[] bytes = new byte[1000];
						bytesRead = (long) fsr.Read(bytes, 0,bytes.Length);
						io.Write(bytes, 0, (int) bytesRead);
						offset += bytesRead;
					}
		}

		public static void Main (string[] args)
		{

		Console.WriteLine ("Server starts...");
		new file_server(args);

		}
	}
}
