//---------------------------------------------------------------------------
// Copyright (c) 2016 Michael G. Brehm
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------

using System;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Reflection;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace zuki.io.compression.test
{
	[TestClass()]
	public class TestLz4
	{
		static byte[] s_sampledata;

		[ClassInitialize()]
		public static void ClassInit(TestContext context)
		{
			// Load the sample data into a byte[] array to use for the unit tests
			using (StreamReader reader = new StreamReader(Assembly.GetExecutingAssembly().GetManifestResourceStream("zuki.io.compression.test.thethreemusketeers.txt")))
			{
				s_sampledata = Encoding.ASCII.GetBytes(reader.ReadToEnd());
			}
		}

		[TestMethod(), TestCategory("Lz4")]
		public void Lz4_CompressDecompress()
		{
			// Start with a MemoryStream created from the sample data
			using (MemoryStream source = new MemoryStream(s_sampledata))
			{
				using (MemoryStream dest = new MemoryStream())
				{
					// Compress the data into the destination memory stream instance
					using (Lz4Writer compressor = new Lz4Writer(dest, CompressionLevel.Optimal, true)) source.CopyTo(compressor);

					// The compressed data should be smaller than the source data
					Assert.IsTrue(dest.Length < source.Length);

					source.SetLength(0);            // Clear the source stream
					dest.Position = 0;              // Reset the destination stream

					// Decompress the data back into the source memory stream
					using (Lz4Reader decompressor = new Lz4Reader(dest, true)) decompressor.CopyTo(source);

					// Ensure that the original data has been restored
					Assert.AreEqual(source.Length, s_sampledata.Length);
					Assert.IsTrue(s_sampledata.SequenceEqual(source.ToArray()));
				}
			}
		}

		[TestMethod(), TestCategory("Lz4")]
		public void Lz4_CompressionLevel()
		{
			using (MemoryStream source = new MemoryStream(s_sampledata))
			{
				long fastest, optimal;          // Size of compressed streams

				// CompressionLevel.NoCompression is not valid
				try { using (Lz4Writer stream = new Lz4Writer(source, CompressionLevel.NoCompression)) Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

				// Compress using CompressionLevel.Fastest
				using (MemoryStream compressed = new MemoryStream())
				{
					using (Lz4Writer compressor = new Lz4Writer(compressed, CompressionLevel.Fastest))
					{
						compressor.Write(s_sampledata, 0, s_sampledata.Length);
						compressor.Flush();
						fastest = compressed.Length;
					}
				}

				// Compress using CompressionLevel.Optimal
				using (MemoryStream compressed = new MemoryStream())
				{
					using (Lz4Writer compressor = new Lz4Writer(compressed, CompressionLevel.Optimal))
					{
						compressor.Write(s_sampledata, 0, s_sampledata.Length);
						compressor.Flush();
						optimal = compressed.Length;
					}
				}

				// Optimal compression should result in a smaller output stream than fastest
				// and fastest should result in a smaller output stream than uncompressed
				Assert.IsTrue(optimal < fastest);
				Assert.IsTrue(fastest < source.Length);
			}
		}

		[TestMethod(), TestCategory("Lz4")]
		public void Lz4_Dispose()
		{
			byte[] buffer = new byte[8192];         // 8KiB data buffer

			// Create a dummy stream and immediately dispose of it
			Lz4Writer stream = new Lz4Writer(new MemoryStream(s_sampledata), CompressionLevel.Optimal);
			stream.Dispose();

			// All properties and methods should throw an ObjectDisposedException
			try { var bs = stream.BaseStream; Assert.Fail("Property access should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { var b = stream.CanRead; Assert.Fail("Property access should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { var b = stream.CanSeek; Assert.Fail("Property access should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { var b = stream.CanWrite; Assert.Fail("Property access should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { stream.Flush(); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { var l = stream.Length; Assert.Fail("Property access should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { var l = stream.Position; Assert.Fail("Property access should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { stream.Position = 12345L; Assert.Fail("Property access should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { stream.Read(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { stream.Seek(0, SeekOrigin.Current); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { stream.SetLength(12345L); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { stream.Write(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			// Ensure that an underlying stream is disposed of properly if leaveopen is not set
			MemoryStream ms = new MemoryStream(s_sampledata);
			using (Lz4Writer compressor = new Lz4Writer(ms, CompressionLevel.Fastest)) { }
			try { ms.Write(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			// Ensure that an underlying stream is not disposed of if leaveopen is set
			ms = new MemoryStream(s_sampledata);
			using (Lz4Writer compressor = new Lz4Writer(ms, CompressionLevel.Fastest, true)) { }
			ms.Write(buffer, 0, 8192);
			ms.Dispose();
		}

		[TestMethod(), TestCategory("Lz4")]
		public void Lz4_BaseStream()
		{
			using (MemoryStream source = new MemoryStream())
			{
				using (Lz4Writer stream = new Lz4Writer(source, CompressionLevel.Optimal))
				{
					Assert.IsNotNull(stream.BaseStream);
					Assert.AreSame(source, stream.BaseStream);
				}
			}
		}

		[TestMethod(), TestCategory("Lz4")]
		public void Lz4_CanRead()
		{
			// Verify behavior of a compression stream
			using (Lz4Writer stream = new Lz4Writer(new MemoryStream()))
			{
				Assert.IsFalse(stream.CanRead);
			}

			// Verify behavior of a decompression stream
			using (Lz4Reader stream = new Lz4Reader(new MemoryStream(s_sampledata)))
			{
				Assert.IsTrue(stream.CanRead);
			}
		}

		[TestMethod(), TestCategory("Lz4")]
		public void Lz4_CanSeek()
		{
			// Verify behavior of a compression stream
			using (Lz4Writer stream = new Lz4Writer(new MemoryStream()))
			{
				Assert.IsFalse(stream.CanSeek);
			}

			// Verify behavior of a decompression stream
			using (Lz4Reader stream = new Lz4Reader(new MemoryStream(s_sampledata)))
			{
				Assert.IsFalse(stream.CanSeek);
			}
		}

		[TestMethod(), TestCategory("Lz4")]
		public void Lz4_CanWrite()
		{
			// Verify behavior of a compression stream
			using (Lz4Writer stream = new Lz4Writer(new MemoryStream()))
			{
				Assert.IsTrue(stream.CanWrite);
			}

			// Verify behavior of a decompression stream
			using (Lz4Reader stream = new Lz4Reader(new MemoryStream(s_sampledata)))
			{
				Assert.IsFalse(stream.CanWrite);
			}
		}

		[TestMethod(), TestCategory("Lz4")]
		public void Lz4_Length()
		{
			// Verify behavior of a compression stream
			using (Lz4Writer stream = new Lz4Writer(new MemoryStream()))
			{
				try { var l = stream.Length; Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}

			// Verify behavior of a decompression stream
			using (Lz4Reader stream = new Lz4Reader(new MemoryStream(s_sampledata)))
			{
				try { var l = stream.Length; Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}
		}

		[TestMethod(), TestCategory("Lz4")]
		public void Lz4_Flush()
		{
			// Verify behavior of flushing a compression stream
			using (MemoryStream compressed = new MemoryStream())
			{
				using (Lz4Writer stream = new Lz4Writer(compressed, true))
				{
					stream.Write(s_sampledata, 0, s_sampledata.Length);

					// Get the unflushed length of the compressed stream and flush it
					long unflushed = compressed.Length;
					stream.Flush();

					// The expectation is that the output stream will be longer after the flush
					long flushedonce = compressed.Length;
					Assert.IsTrue(compressed.Length > unflushed);

					// Flushing the same data a second time should not have any impact at all
					stream.Flush();
					Assert.AreEqual(compressed.Length, flushedonce);

					// The stream should still be writable after a flush operation
					stream.Write(s_sampledata, 0, s_sampledata.Length / 10);
				}
			}

			// Verify behavior of flushing a decompression stream
			using (Lz4Reader stream = new Lz4Reader(new MemoryStream(s_sampledata)))
			{
				// Flush has no effect on decompression streams, just ensure it doesn't throw
				stream.Flush();
			}
		}

		[TestMethod(), TestCategory("Lz4")]
		public void Lz4_Position()
		{
			// Start with a MemoryStream created from the sample data
			using (MemoryStream source = new MemoryStream(s_sampledata))
			{
				using (MemoryStream dest = new MemoryStream())
				{
					// Test a compression stream
					using (Lz4Writer compressor = new Lz4Writer(dest, CompressionLevel.Optimal, true))
					{
						// Lz4 streams do not report position
						try { var pos = compressor.Position; Assert.Fail("Property should have thrown an exception"); }
						catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

						// Attempting to set the position on the stream should throw
						try { compressor.Position = 12345L; Assert.Fail("Property should have thrown an exception"); }
						catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
					}

					source.SetLength(0);            // Clear the source stream
					dest.Position = 0;              // Reset the destination stream

					// Test a decompression stream
					using (Lz4Reader decompressor = new Lz4Reader(dest, true))
					{
						// Lz4 streams do not report position
						try { var pos = decompressor.Position; Assert.Fail("Property should have thrown an exception"); }
						catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

						// Attempting to set the position on the stream should throw
						try { decompressor.Position = 12345L; Assert.Fail("Property should have thrown an exception"); }
						catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
					}
				}
			}
		}

		[TestMethod(), TestCategory("Lz4")]
		public void Lz4_Read()
		{
			byte[] buffer = new byte[8192];         // 8KiB data buffer

			using (MemoryStream compressed = new MemoryStream())
			{
				// Start with a compressed MemoryStream created from the sample data
				using (Lz4Writer compressor = new Lz4Writer(compressed, CompressionLevel.Optimal, true))
				{
					compressor.Write(s_sampledata, 0, s_sampledata.Length);
					compressor.Flush();
				}

				compressed.Position = 0;
				using (Stream file = File.Create("D:\\test.lz4")) { compressed.CopyTo(file); file.Flush(); }

				// Create a decompressor to test some of the error cases
				using (Lz4Reader decompressor = new Lz4Reader(compressed, true))
				{
					// Send in some bum arguments to Read() to check they are caught
					try { decompressor.Read(null, 0, 0); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

					try { decompressor.Read(buffer, -1, 0); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

					try { decompressor.Read(buffer, 0, -1); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

					try { decompressor.Read(buffer, 0, buffer.Length + 1024); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentException)); }

					// Attempting to read from the end of the compressed stream should throw an InvalidDataException
					try { decompressor.Read(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(InvalidDataException)); }

					// Attempting to read from the middle of the compressed stream should throw an Lz4Exception
					compressed.Position = compressed.Position / 2;
					try { decompressor.Read(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(Lz4Exception)); }
				}

				// Create a new decompressor against the same stream and make sure it doesn't throw
				compressed.Position = 0;
				using (Lz4Reader decompressor = new Lz4Reader(compressed, true))
				{
					while (decompressor.Read(buffer, 0, 8192) != 0) { }
				}
			}
		}

		[TestMethod(), TestCategory("Lz4")]
		public void Lz4_Seek()
		{
			// Verify behavior of a compression stream
			using (Lz4Writer stream = new Lz4Writer(new MemoryStream()))
			{
				try { stream.Seek(50, SeekOrigin.Begin); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

				try { stream.Seek(0, SeekOrigin.Current); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

				try { stream.Seek(-50, SeekOrigin.End); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}

			// Verify behavior of a decompression stream
			using (Lz4Reader stream = new Lz4Reader(new MemoryStream(s_sampledata)))
			{
				try { stream.Seek(50, SeekOrigin.Begin); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

				try { stream.Seek(0, SeekOrigin.Current); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

				try { stream.Seek(-50, SeekOrigin.End); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}
		}

		[TestMethod(), TestCategory("Lz4")]
		public void Lz4_SetLength()
		{
			// Verify behavior of a compression stream
			using (Lz4Writer stream = new Lz4Writer(new MemoryStream()))
			{
				try { stream.SetLength(12345L); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}

			// Verify behavior of a decompression stream
			using (Lz4Reader stream = new Lz4Reader(new MemoryStream(s_sampledata)))
			{
				try { stream.SetLength(12345L); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}
		}

		[TestMethod(), TestCategory("Lz4")]
		public void Lz4_Write()
		{
			byte[] buffer = new byte[8192];         // 8KiB data buffer

			// Compress the sample data using a call to Write directly
			using (MemoryStream compressed = new MemoryStream())
			{
				using (Lz4Writer compressor = new Lz4Writer(compressed, CompressionLevel.Optimal, true))
				{
					// Send in some bum arguments to Write() to check they are caught
					try { compressor.Write(null, 0, 0); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

					try { compressor.Write(s_sampledata, -1, 0); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

					try { compressor.Write(s_sampledata, 0, -1); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

					try { compressor.Write(s_sampledata, 0, s_sampledata.Length + 1024); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentException)); }

					// Compress the data; there really isn't much that can go wrong with Write() itself
					compressor.Write(s_sampledata, 0, s_sampledata.Length);
					compressor.Flush();
				}
			}
		}
	}
}
