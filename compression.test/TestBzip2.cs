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
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace zuki.io.compression.test
{
	[TestClass()]
	public class TestBzip2
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

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_CompressExternal()
		{
			// This method generates an output file that can be tested externally; "thethreemusketeers.txt" is
			// set to Copy Always to the output directory, it can be diffed after running the external tool
			using (Bzip2Writer writer = new Bzip2Writer(File.Create(Path.Combine(Environment.CurrentDirectory, "thethreemusketeers.bz2"))))
			{
				writer.Write(s_sampledata);
				writer.Flush();
			}
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_DecompressExternal()
		{
			// Decompress a stream created externally to this library
			using (Bzip2Reader reader = new Bzip2Reader(Assembly.GetExecutingAssembly().GetManifestResourceStream("zuki.io.compression.test.thethreemusketeers.bz2")))
			{
				using (MemoryStream dest = new MemoryStream())
				{
					reader.CopyTo(dest);
					dest.Flush();

					// Verify that the output matches the sample data byte-for-byte
					Assert.IsTrue(Enumerable.SequenceEqual(s_sampledata, dest.ToArray()));
				}
			}
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_CompressDecompress()
		{
			// Start with a MemoryStream created from the sample data
			using (MemoryStream source = new MemoryStream(s_sampledata))
			{
				using (MemoryStream dest = new MemoryStream())
				{
					// Compress the data into the destination memory stream instance
					using (Bzip2Writer compressor = new Bzip2Writer(dest, CompressionLevel.Optimal, true)) source.CopyTo(compressor);

					// The compressed data should be smaller than the source data
					Assert.IsTrue(dest.Length < source.Length);

					source.SetLength(0);			// Clear the source stream
					dest.Position = 0;				// Reset the destination stream

					// Decompress the data back into the source memory stream
					using (Bzip2Reader decompressor = new Bzip2Reader(dest, true)) decompressor.CopyTo(source);

					// Ensure that the original data has been restored
					Assert.AreEqual(source.Length, s_sampledata.Length);
					Assert.IsTrue(s_sampledata.SequenceEqual(source.ToArray()));
				}
			}
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_CompressionLevel()
		{
			using (MemoryStream source = new MemoryStream(s_sampledata))
			{
				long fastest, optimal;				// Size of compressed streams

				// CompressionLevel.NoCompression is not valid
				try { using (Bzip2Writer stream = new Bzip2Writer(source, CompressionLevel.NoCompression)) Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

				// Compress using CompressionLevel.Fastest
				using (MemoryStream compressed = new MemoryStream())
				{
					using (Bzip2Writer compressor = new Bzip2Writer(compressed, CompressionLevel.Fastest))
					{
						compressor.Write(s_sampledata, 0, s_sampledata.Length);
						compressor.Flush();
						fastest = compressed.Length;
					}
				}

				// Compress using CompressionLevel.Optimal
				using (MemoryStream compressed = new MemoryStream())
				{
					using (Bzip2Writer compressor = new Bzip2Writer(compressed, CompressionLevel.Optimal))
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

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_ReaderDispose()
		{
			byte[] buffer = new byte[8192];         // 8KiB data buffer

			// Create a dummy stream and immediately dispose of it
			Bzip2Reader stream = new Bzip2Reader(new MemoryStream(s_sampledata));
			stream.Dispose();

			// Test double dispose
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
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_WriterDispose()
		{
			byte[] buffer = new byte[8192];         // 8KiB data buffer

			// Create a dummy stream and immediately dispose of it
			Bzip2Writer stream = new Bzip2Writer(new MemoryStream(s_sampledata), CompressionLevel.Optimal);
			stream.Dispose();

			// Test double dispose
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

			try { stream.Write(buffer); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { stream.Write(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			// Ensure that an underlying stream is disposed of properly if leaveopen is not set
			MemoryStream ms = new MemoryStream(s_sampledata);
			using (Bzip2Writer compressor = new Bzip2Writer(ms, CompressionLevel.Fastest)) { }
			try { ms.Write(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			// Ensure that an underlying stream is not disposed of if leaveopen is set
			ms = new MemoryStream(s_sampledata);
			using (Bzip2Writer compressor = new Bzip2Writer(ms, CompressionLevel.Fastest, true)) { }
			ms.Write(buffer, 0, 8192);
			ms.Dispose();
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_BaseStream()
		{
			using (MemoryStream source = new MemoryStream())
			{
				using (Bzip2Writer stream = new Bzip2Writer(source, CompressionLevel.Optimal))
				{
					Assert.IsNotNull(stream.BaseStream);
					Assert.AreSame(source, stream.BaseStream);
				}

				using (Bzip2Reader stream = new Bzip2Reader(source, true))
				{
					Assert.IsNotNull(stream.BaseStream);
					Assert.AreSame(source, stream.BaseStream);
				}
			}
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_CanRead()
		{
			// Verify behavior of a compression stream
			using (Bzip2Writer stream = new Bzip2Writer(new MemoryStream()))
			{
				Assert.IsFalse(stream.CanRead);
			}

			// Verify behavior of a decompression stream
			using (Bzip2Reader stream = new Bzip2Reader(new MemoryStream(s_sampledata)))
			{
				Assert.IsTrue(stream.CanRead);
			}
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_CanSeek()
		{
			// Verify behavior of a compression stream
			using (Bzip2Writer stream = new Bzip2Writer(new MemoryStream()))
			{
				Assert.IsFalse(stream.CanSeek);
			}

			// Verify behavior of a decompression stream
			using (Bzip2Reader stream = new Bzip2Reader(new MemoryStream(s_sampledata)))
			{
				Assert.IsFalse(stream.CanSeek);
			}
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_CanWrite()
		{
			// Verify behavior of a compression stream
			using (Bzip2Writer stream = new Bzip2Writer(new MemoryStream()))
			{
				Assert.IsTrue(stream.CanWrite);
			}

			// Verify behavior of a decompression stream
			using (Bzip2Reader stream = new Bzip2Reader(new MemoryStream(s_sampledata)))
			{
				Assert.IsFalse(stream.CanWrite);
			}
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_Length()
		{
			// Verify behavior of a compression stream
			using (Bzip2Writer stream = new Bzip2Writer(new MemoryStream()))
			{
				try { var l = stream.Length; Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}

			// Verify behavior of a decompression stream
			using (Bzip2Reader stream = new Bzip2Reader(new MemoryStream(s_sampledata)))
			{
				try { var l = stream.Length; Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_Flush()
		{
			// Verify behavior of flushing a compression stream
			using (MemoryStream compressed = new MemoryStream())
			{
				using (Bzip2Writer stream = new Bzip2Writer(compressed, true))
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
			using (Bzip2Reader stream = new Bzip2Reader(new MemoryStream(s_sampledata)))
			{
				// Flush has no effect on decompression streams, just ensure it doesn't throw
				stream.Flush();
			}
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_Position()
		{
			// Start with a MemoryStream created from the sample data
			using (MemoryStream source = new MemoryStream(s_sampledata))
			{
				using (MemoryStream dest = new MemoryStream())
				{
					// Test a compression stream
					using (Bzip2Writer compressor = new Bzip2Writer(dest, CompressionLevel.Optimal, true))
					{
						// The stream should report position zero prior to compression
						Assert.AreEqual(0L, compressor.Position);
						source.CopyTo(compressor);

						// The stream should report non-zero after compression
						Assert.AreNotEqual(0L, compressor.Position);

						// Attempting to set the position on the stream should throw
						try { compressor.Position = 12345L; Assert.Fail("Property should have thrown an exception"); }
						catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
					}

					source.SetLength(0);            // Clear the source stream
					dest.Position = 0;              // Reset the destination stream

					// Test a decompression stream
					using (Bzip2Reader decompressor = new Bzip2Reader(dest, true))
					{
						// The stream should report position zero prior to compression
						Assert.AreEqual(0L, decompressor.Position);
						decompressor.CopyTo(source);
						
						// The stream should report non-zero after compression
						Assert.AreNotEqual(0L, decompressor.Position);

						// Attempting to set the position on the stream should throw
						try { decompressor.Position = 12345L; Assert.Fail("Property should have thrown an exception"); }
						catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
					}
				}
			}
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_Read()
		{
			byte[] buffer = new byte[8192];			// 8KiB data buffer

			using (MemoryStream compressed = new MemoryStream())
			{
				// Start with a compressed MemoryStream created from the sample data
				using (Bzip2Writer compressor = new Bzip2Writer(compressed, CompressionLevel.Optimal, true))
				{
					try { compressor.Read(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

					compressor.Write(s_sampledata, 0, s_sampledata.Length);
					compressor.Flush();
				}

				// Check the constructor for ArgumentNullException while we're here
				try { using (Bzip2Reader decompressor = new Bzip2Reader(null, false)) { }; Assert.Fail("Constructor should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

				// Create a decompressor to test some of the error cases
				using (Bzip2Reader decompressor = new Bzip2Reader(compressed, true))
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

					// Attempting to read from the middle of the compressed stream should throw a Bzip2Exception
					compressed.Position = compressed.Position / 2;
					try { decompressor.Read(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(Bzip2Exception)); }

					// The decompression stream is trashed at this point since the input buffer was filled
					// with data from the middle.  Thought about a special case handler for that, but it's
					// a fringe case.  Verify that the stream is indeed trashed ...
					compressed.Position = 0;
					try { decompressor.Read(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(Bzip2Exception)); }
				}

				// Create a new decompressor against the same stream and make sure it doesn't throw
				using (Bzip2Reader decompressor = new Bzip2Reader(compressed, true))
				{
					// Reading zero bytes should not throw an exception
					decompressor.Read(buffer, 0, 0);

					while (decompressor.Read(buffer, 0, 8192) != 0) { }
				}
			}
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_Seek()
		{
			// Verify behavior of a compression stream
			using (Bzip2Writer stream = new Bzip2Writer(new MemoryStream()))
			{
				try { stream.Seek(50, SeekOrigin.Begin); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

				try { stream.Seek(0, SeekOrigin.Current); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

				try { stream.Seek(-50, SeekOrigin.End); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}

			// Verify behavior of a decompression stream
			using (Bzip2Reader stream = new Bzip2Reader(new MemoryStream(s_sampledata)))
			{
				try { stream.Seek(50, SeekOrigin.Begin); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

				try { stream.Seek(0, SeekOrigin.Current); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

				try { stream.Seek(-50, SeekOrigin.End); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_SetLength()
		{
			// Verify behavior of a compression stream
			using (Bzip2Writer stream = new Bzip2Writer(new MemoryStream()))
			{
				try { stream.SetLength(12345L); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}

			// Verify behavior of a decompression stream
			using (Bzip2Reader stream = new Bzip2Reader(new MemoryStream(s_sampledata)))
			{
				try { stream.SetLength(12345L); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_Write()
		{
			byte[] buffer = new byte[8192];         // 8KiB data buffer

			// Compress the sample data using a call to Write directly
			using (MemoryStream compressed = new MemoryStream())
			{
				// Check the constructor for ArgumentNullException while we're here
				try { using (Bzip2Writer compressor = new Bzip2Writer(null)) { }; Assert.Fail("Constructor should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

				using (Bzip2Writer compressor = new Bzip2Writer(compressed, CompressionLevel.Optimal, true))
				{
					// Send in some bum arguments to Write() to check they are caught
					try { compressor.Write(null); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

					try { compressor.Write(null, 0, 0); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

					try { compressor.Write(s_sampledata, -1, 0); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

					try { compressor.Write(s_sampledata, 0, -1); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

					try { compressor.Write(s_sampledata, 0, s_sampledata.Length + 1024); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentException)); }

					// Not writing anything shouldn't throw an exception
					compressor.Write(s_sampledata, 0, 0);

					// Compress the data; there really isn't much that can go wrong with Write() itself
					compressor.Write(s_sampledata, 0, s_sampledata.Length);
					compressor.Flush();
				}

				using (Bzip2Reader reader = new Bzip2Reader(compressed, true))
				{
					try { reader.Write(buffer, 0, buffer.Length); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
				}
			}
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_Bzip2Exception()
		{
			using (MemoryStream compressed = new MemoryStream())
			{
				// Start with a compressed MemoryStream created from the sample data
				using (Bzip2Writer compressor = new Bzip2Writer(compressed, CompressionLevel.Optimal, true))
				{
					compressor.Write(s_sampledata, 0, s_sampledata.Length);
					compressor.Flush();
				}

				byte[] buffer = new byte[8192];
				Bzip2Exception thrown = null;
				Bzip2Exception deserialized = null;

				// Create a decompressor to test exception cases
				using (Bzip2Reader decompressor = new Bzip2Reader(compressed, true))
				{
					// Attempting to read from the middle of the compressed stream should throw a Bzip2Exception
					compressed.Position = compressed.Length / 2;
					try { decompressor.Read(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
					catch (Bzip2Exception ex) { thrown = ex; }

					Assert.IsNotNull(thrown);
					Assert.IsInstanceOfType(thrown, typeof(Bzip2Exception));

					// Check the error code property
					Assert.AreEqual(-5, thrown.ErrorCode);      // BZ_DATA_ERROR_MAGIC (-5)

					// Serialize and de-serialize the exception with a BinaryFormatter
					BinaryFormatter formatter = new BinaryFormatter();
					using (MemoryStream memstream = new MemoryStream())
					{
						formatter.Serialize(memstream, thrown);
						memstream.Seek(0, 0);
						deserialized = (Bzip2Exception)formatter.Deserialize(memstream);
					}

					// Check that the exceptions are equivalent
					Assert.AreEqual(thrown.ErrorCode, deserialized.ErrorCode);
					Assert.AreEqual(thrown.StackTrace, deserialized.StackTrace);
					Assert.AreEqual(thrown.ToString(), deserialized.ToString());
				}
			}
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_Bzip2CompressionLevel()
		{
			// Constructors
			var level = new Bzip2CompressionLevel(5);
			Assert.AreEqual(5, level);

			level = new Bzip2CompressionLevel(CompressionLevel.Fastest);
			Assert.AreEqual(1, level);

			level = new Bzip2CompressionLevel(CompressionLevel.Optimal);
			Assert.AreEqual(9, level);

			try { level = new Bzip2CompressionLevel(14); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { level = new Bzip2CompressionLevel(CompressionLevel.NoCompression); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { level = new Bzip2CompressionLevel((CompressionLevel)99); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			// Implicit conversion
			level = 8;
			Assert.AreEqual(8, level);

			// Equality
			Assert.IsTrue(new Bzip2CompressionLevel(8) == level);
			Assert.IsFalse(new Bzip2CompressionLevel(1) == level);
			Assert.IsTrue(new Bzip2CompressionLevel(3) != level);
			Assert.IsFalse(new Bzip2CompressionLevel(8) != level);

			object o = new Bzip2CompressionLevel(2);
			Assert.IsTrue(o.Equals(new Bzip2CompressionLevel(2)));
			Assert.IsFalse(o.Equals(new Bzip2CompressionLevel(9)));
			Assert.IsTrue(new Bzip2CompressionLevel(2).Equals(new Bzip2CompressionLevel(2)));
			Assert.IsFalse(new Bzip2CompressionLevel(8).Equals(new Bzip2CompressionLevel(2)));
			Assert.IsFalse(o.Equals(null));
			Assert.IsFalse(o.Equals(new Bzip2WorkFactor(88)));

			// HashCode
			int hash = new Bzip2CompressionLevel(CompressionLevel.Fastest).GetHashCode();
			Assert.AreNotEqual(0, hash);

			// ToString
			string s = new Bzip2CompressionLevel(9).ToString();
			Assert.IsFalse(String.IsNullOrEmpty(s));
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_Bzip2WorkFactor()
		{
			// Constructors
			var factor = new Bzip2WorkFactor(30);
			Assert.AreEqual(30, factor);

			try { factor = new Bzip2WorkFactor(300); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			// Implicit conversion
			factor = 80;
			Assert.AreEqual(80, factor);

			// Equality
			Assert.IsTrue(new Bzip2WorkFactor(80) == factor);
			Assert.IsFalse(new Bzip2WorkFactor(10) == factor);
			Assert.IsTrue(new Bzip2WorkFactor(30) != factor);
			Assert.IsFalse(new Bzip2WorkFactor(80) != factor);

			object o = new Bzip2WorkFactor(20);
			Assert.IsTrue(o.Equals(new Bzip2WorkFactor(20)));
			Assert.IsFalse(o.Equals(new Bzip2WorkFactor(90)));
			Assert.IsTrue(new Bzip2WorkFactor(20).Equals(new Bzip2WorkFactor(20)));
			Assert.IsFalse(new Bzip2WorkFactor(80).Equals(new Bzip2WorkFactor(20)));
			Assert.IsFalse(o.Equals(null));
			Assert.IsFalse(o.Equals(new Bzip2CompressionLevel(8)));

			// HashCode
			int hash = new Bzip2WorkFactor(33).GetHashCode();
			Assert.AreNotEqual(0, hash);

			// ToString
			string s = new Bzip2WorkFactor(88).ToString();
			Assert.IsFalse(String.IsNullOrEmpty(s));
		}

		[TestMethod(), TestCategory("Bzip2")]
		public void Bzip2_Encoder()
		{
			// The Bzip2Encoder is just a wrapper around Bzip2Writer that provides 
			// more complete control over the compression/encoder parameters
			Bzip2Encoder encoder = new Bzip2Encoder();

			// Check the default values
			Assert.AreEqual(65536, encoder.BufferSize);
			Assert.AreEqual(Bzip2CompressionLevel.Default, encoder.CompressionLevel);
			Assert.AreEqual(Bzip2WorkFactor.Default, encoder.WorkFactor);

			// Set some bad values and ensure they are caught by the encoder property setters
			try { encoder.BufferSize = -1; Assert.Fail("Property should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { encoder.CompressionLevel = -1; Assert.Fail("Property should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { encoder.CompressionLevel = 0; Assert.Fail("Property should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { encoder.CompressionLevel = 10; Assert.Fail("Property should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { encoder.WorkFactor = new Bzip2WorkFactor(-1); Assert.Fail("Property should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { encoder.WorkFactor = new Bzip2WorkFactor(251); Assert.Fail("Property should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			// Set some good values
			encoder.BufferSize = 8192;
			Assert.AreEqual(8192, encoder.BufferSize);

			encoder.CompressionLevel = Bzip2CompressionLevel.Fastest;
			Assert.AreEqual(Bzip2CompressionLevel.Fastest, encoder.CompressionLevel);

			encoder.WorkFactor = 100;
			Assert.AreEqual(100, encoder.WorkFactor);

			// Check all of the Encoder methods work and encode as expected
			byte[] expected, actual;
			using (MemoryStream ms = new MemoryStream())
			{
				using (var writer = new Bzip2Writer(ms, CompressionLevel.Fastest, true)) writer.Write(s_sampledata, 0, s_sampledata.Length);
				expected = ms.ToArray();
			}

			// Check parameter validations
			try { actual = encoder.Encode((byte[])null); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			try { actual = encoder.Encode((Stream)null); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			try { actual = encoder.Encode(null, 0, 0); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			try { encoder.Encode(s_sampledata, null); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			try { encoder.Encode((byte[])null, new MemoryStream()); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			try { encoder.Encode((byte[])null, 0, 0, new MemoryStream()); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			try { encoder.Encode(s_sampledata, 0, s_sampledata.Length, (Stream)null); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			try { encoder.Encode((Stream)null, new MemoryStream()); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			try { encoder.Encode(new MemoryStream(), (Stream)null); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			// Check actual encoding
			actual = encoder.Encode(s_sampledata);
			Assert.IsTrue(Enumerable.SequenceEqual(expected, actual));

			actual = encoder.Encode(new MemoryStream(s_sampledata));
			Assert.IsTrue(Enumerable.SequenceEqual(expected, actual));

			actual = encoder.Encode(s_sampledata, 0, s_sampledata.Length);
			Assert.IsTrue(Enumerable.SequenceEqual(expected, actual));

			using (MemoryStream dest = new MemoryStream())
			{
				encoder.Encode(s_sampledata, dest);
				Assert.IsTrue(Enumerable.SequenceEqual(expected, dest.ToArray()));
			}

			using (MemoryStream dest = new MemoryStream())
			{
				encoder.Encode(new MemoryStream(s_sampledata), dest);
				Assert.IsTrue(Enumerable.SequenceEqual(expected, dest.ToArray()));
			}

			using (MemoryStream dest = new MemoryStream())
			{
				encoder.Encode(s_sampledata, 0, s_sampledata.Length, dest);
				Assert.IsTrue(Enumerable.SequenceEqual(expected, dest.ToArray()));
			}
		}
	}
}
