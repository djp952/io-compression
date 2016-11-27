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
using System.Linq;
using System.Reflection;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace zuki.io.compression.test
{
	[TestClass()]
	public class TestXz
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

		[TestMethod(), TestCategory("Xz")]
		public void Xz_DecompressExternal()
		{
			// Decompress a stream created externally to this library
			using (XzReader reader = new XzReader(Assembly.GetExecutingAssembly().GetManifestResourceStream("zuki.io.compression.test.thethreemusketeers.xz")))
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

		[TestMethod(), TestCategory("Xz")]
		public void Xz_CompressDecompress()
		{
			// Start with a MemoryStream created from the sample data
			using (MemoryStream source = new MemoryStream(s_sampledata))
			{
				using (MemoryStream dest = new MemoryStream())
				{
					// Compress the data into the destination memory stream instance
					XzEncoder encoder = new XzEncoder();
					encoder.Encode(source, dest);

					// The compressed data should be smaller than the source data
					Assert.IsTrue(dest.Length < source.Length);

					source.SetLength(0);            // Clear the source stream
					dest.Position = 0;              // Reset the destination stream

					// Decompress the data back into the source memory stream
					using (XzReader decompressor = new XzReader(dest, true)) decompressor.CopyTo(source);

					// Ensure that the original data has been restored
					Assert.AreEqual(source.Length, s_sampledata.Length);
					Assert.IsTrue(s_sampledata.SequenceEqual(source.ToArray()));
				}
			}
		}

		[TestMethod(), TestCategory("Xz")]
		public void Xz_Dispose()
		{
			byte[] buffer = new byte[8192];         // 8KiB data buffer

			// Create a memorystream to hold the compressed sample data
			using (MemoryStream ms = new MemoryStream())
			{
				XzEncoder encoder = new XzEncoder();
				encoder.Encode(s_sampledata, ms);

				// Create a decompression stream and immediately dispose of it
				XzReader stream = new XzReader(ms);
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
		}

		[TestMethod(), TestCategory("Xz")]
		public void Xz_BaseStream()
		{
			using (MemoryStream dest = new MemoryStream())
			{
				XzEncoder encoder = new XzEncoder();
				encoder.Encode(s_sampledata, dest);
				using (XzReader stream = new XzReader(dest))
				{
					Assert.IsNotNull(stream.BaseStream);
					Assert.AreSame(dest, stream.BaseStream);
				}
			}
		}

		[TestMethod(), TestCategory("Xz")]
		public void Xz_CanRead()
		{
			// Verify behavior of a decompression stream
			using (XzReader stream = new XzReader(new MemoryStream(s_sampledata)))
			{
				Assert.IsTrue(stream.CanRead);
			}
		}

		[TestMethod(), TestCategory("Xz")]
		public void Xz_CanSeek()
		{
			// Verify behavior of a decompression stream
			using (XzReader stream = new XzReader(new MemoryStream(s_sampledata)))
			{
				Assert.IsFalse(stream.CanSeek);
			}
		}

		[TestMethod(), TestCategory("Xz")]
		public void Xz_CanWrite()
		{
			// Verify behavior of a decompression stream
			using (XzReader stream = new XzReader(new MemoryStream(s_sampledata)))
			{
				Assert.IsFalse(stream.CanWrite);
			}
		}

		[TestMethod(), TestCategory("Xz")]
		public void Xz_Length()
		{
			// Verify behavior of a decompression stream
			using (XzReader stream = new XzReader(new MemoryStream(s_sampledata)))
			{
				try { var l = stream.Length; Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}
		}

		[TestMethod(), TestCategory("Xz")]
		public void Xz_Flush()
		{
			// Verify behavior of flushing a decompression stream
			using (XzReader stream = new XzReader(new MemoryStream(s_sampledata)))
			{
				// Flush has no effect on decompression streams, just ensure it doesn't throw
				stream.Flush();
			}
		}

		[TestMethod(), TestCategory("Xz")]
		public void Xz_Read()
		{
			byte[] buffer = new byte[8192];         // 8KiB data buffer

			using (MemoryStream compressed = new MemoryStream())
			{
				XzEncoder encoder = new XzEncoder();
				encoder.Encode(s_sampledata, compressed);
				compressed.Flush();

				// Create a decompressor to test some of the error cases
				using (XzReader decompressor = new XzReader(compressed, true))
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
				}

				// Create a new decompressor against the same stream and make sure it doesn't throw
				compressed.Position = 0;
				using (XzReader decompressor = new XzReader(compressed, true))
				{
					while (decompressor.Read(buffer, 0, 8192) != 0) { }
				}
			}
		}

		[TestMethod(), TestCategory("Xz")]
		public void Xz_Seek()
		{
			// Verify behavior of a decompression stream
			using (XzReader stream = new XzReader(new MemoryStream(s_sampledata)))
			{
				try { stream.Seek(50, SeekOrigin.Begin); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

				try { stream.Seek(0, SeekOrigin.Current); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

				try { stream.Seek(-50, SeekOrigin.End); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}
		}

		[TestMethod(), TestCategory("Xz")]
		public void Xz_SetLength()
		{
			// Verify behavior of a decompression stream
			using (XzReader stream = new XzReader(new MemoryStream(s_sampledata)))
			{
				try { stream.SetLength(12345L); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}
		}
	}
}
