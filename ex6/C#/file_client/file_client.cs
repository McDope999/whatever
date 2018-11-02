using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Collections;
using tcp;

namespace tcp
{
	class file_client
	{
		/// The PORT
		int PORT = 9000;
			/// The BUFSIZE
		const int BUFSIZE = 1000;
		string localAddress = "10.0.0.1";

		TcpClient client  = null;

		private file_client (string[] args)
		{
					if(args.Length > 0)
						PORT = int.Parse(args[0]);

					if(args.Length > 1)
						localAddress = args[1];


					try
					{

							 client = new TcpClient(localAddress, PORT);


							 System.Console.WriteLine("Enter Filename (path):");
							 string message = System.Console.ReadLine();
							 // Translate the passed message into ASCII and store it as a Byte array.
							//Byte[] data = System.Text.Encoding.ASCII.GetBytes(message);

							 // Get a client stream for reading and writing.
							//  Stream stream = client.GetStream();

							 NetworkStream SocketStream = client.GetStream();

							 // Send the message to the connected TcpServer.
							tcp.LIB.writeTextTCP(SocketStream, message);

							 Console.WriteLine("Sent: {0}", message);

							long fileSize = tcp.LIB.getFileSizeTCP(SocketStream);
							if(fileSize != 0)
							{
								Console.WriteLine($"File length: {fileSize}");
								string fileName = tcp.LIB.extractFileName(message);
								Console.WriteLine($"Trying to receive file..");
								receiveFile(fileName, SocketStream, fileSize);
							}
							else{
								Console.WriteLine($"File did not exist on sever. Closing connection..");
								SocketStream.Close();
 							 client.Close();
							}

				 }
				 catch (ArgumentNullException e)
				 {
					 		Console.WriteLine("ArgumentNullException: {0}", e);
				 }
				 catch (SocketException e)
				 {
					 		Console.WriteLine("SocketException: {0}", e);
				 }

				 Console.WriteLine("\n Press Enter to continue...");
				 Console.Read();

			}

		/// <summary>
		/// Receives the file.
		/// </summary>
		/// <param name='fileName'>
		/// File name.
		/// </param>
		/// <param name='io'>
		/// Network stream for reading from the server
		/// </param>
		private void receiveFile (String fileName, NetworkStream io, long fileSize)
		{

			FileStream fsw = File.OpenWrite(fileName);
			long bytesRead = 0;

			while(fsw.Length < fileSize){
						byte[] bytes = new byte[1000];
						bytesRead = io.Read(bytes, 0, bytes.Length);
						fsw.Write(bytes, 0, (int) bytesRead);
					}

		}

		/// <summary>
		/// The entry point of the program, where the program control starts and ends.
		/// </summary>
		/// <param name='args'>
		/// The command-line arguments.
		/// </param>
		public static void Main (string[] args)
		{
			Console.WriteLine ("Client starts...");
			new file_client(args);
		}
	}
}
