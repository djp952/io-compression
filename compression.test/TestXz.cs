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
using System.Runtime.Serialization.Formatters.Binary;
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
		public void Xz_CompressExternal()
		{
			// This method generates an output file that can be tested externally; "thethreemusketeers.txt" is
			// set to Copy Always to the output directory, it can be diffed after running the external tool
			XzEncoder encoder = new XzEncoder();
			using (var outfile = File.Create(Path.Combine(Environment.CurrentDirectory, "thethreemusketeers.xz")))
			{
				encoder.Encode(s_sampledata, outfile);
				outfile.Flush();
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
		public void Xz_Position()
		{
			// Start with a MemoryStream created from the sample data
			using (MemoryStream compressed = new MemoryStream(s_sampledata))
			{
				XzEncoder encoder = new XzEncoder();
				encoder.Encode(s_sampledata, compressed);

				compressed.Position = 0;
				using (XzReader reader = new XzReader(compressed))
				{
					// Attempting to set the position on the stream should throw
					try { reader.Position = 12345L; Assert.Fail("Property should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

					// Read some data from the stream, position should still throw for XzReader
					byte[] buffer = new byte[8192];
					try { var p = reader.Position; Assert.Fail("Property should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
				}
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

				// Check the constructor for ArgumentNullException while we're here
				try { using (XzReader decompressor = new XzReader(null, false)) { }; Assert.Fail("Constructor should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

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
					// Reading zero bytes should not throw an exception
					decompressor.Read(buffer, 0, 0);

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

		[TestMethod(), TestCategory("Xz")]
		public void Xz_Write()
		{
			XzEncoder encoder = new XzEncoder();
			var compressed = encoder.Encode(s_sampledata);
			using (XzReader reader = new XzReader(new MemoryStream(compressed)))
			{
				byte[] buffer = new byte[2048];
				try { reader.Write(buffer, 0, buffer.Length); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}
		}

		[TestMethod(), TestCategory("Xz")]
		public void Xz_LzmaException()
		{
			using (MemoryStream compressed = new MemoryStream())
			{
				XzEncoder encoder = new XzEncoder();
				encoder.CompressionLevel = LzmaCompressionLevel.Optimal;
				encoder.Encode(s_sampledata, compressed);
				compressed.Flush();

				byte[] buffer = new byte[8192];
				LzmaException thrown = null;
				LzmaException deserialized = null;

				// Create a decompressor to test exception cases
				using (XzReader decompressor = new XzReader(compressed, true))
				{
					// Attempting to read from the middle of the compressed stream should throw a LzmaException
					compressed.Position = compressed.Length / 2;
					try { decompressor.Read(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
					catch (LzmaException ex) { thrown = ex; }

					Assert.IsNotNull(thrown);
					Assert.IsInstanceOfType(thrown, typeof(LzmaException));

					// Check the error code property
					Assert.AreEqual(17, thrown.ErrorCode);      // SZ_ERROR_NO_ARCHIVE (17)

					// Serialize and de-serialize the exception with a BinaryFormatter
					BinaryFormatter formatter = new BinaryFormatter();
					using (MemoryStream memstream = new MemoryStream())
					{
						formatter.Serialize(memstream, thrown);
						memstream.Seek(0, 0);
						deserialized = (LzmaException)formatter.Deserialize(memstream);
					}

					// Check that the exceptions are equivalent
					Assert.AreEqual(thrown.ErrorCode, deserialized.ErrorCode);
					Assert.AreEqual(thrown.StackTrace, deserialized.StackTrace);
					Assert.AreEqual(thrown.ToString(), deserialized.ToString());
				}
			}
		}

		[TestMethod(), TestCategory("Xz")]
		public void Xz_Lzma2BlockSize()
		{
			// Constructors
			var size = new Lzma2BlockSize(65536L);
			Assert.AreEqual(65536L, size);

			try { size = new Lzma2BlockSize(-1L); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			if (IntPtr.Size == 4)
			{
				// block size cannot exceed UInt32::MaxValue on 32-bit builds
				try { size = new Lzma2BlockSize((long)(UInt32.MaxValue) + 1); Assert.Fail("Constructor should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }
			}

			// Implicit conversions
			size = 123456;
			Assert.AreEqual(123456, size);

			size = 8000000L;
			Assert.AreEqual(8000000L, size);

			// Equality
			Assert.IsTrue(new Lzma2BlockSize(8000000L) == size);
			Assert.IsFalse(new Lzma2BlockSize(1000L) == size);
			Assert.IsTrue(new Lzma2BlockSize(3000L) != size);
			Assert.IsFalse(new Lzma2BlockSize(8000000L) != size);

			object o = new Lzma2BlockSize(2000);
			Assert.IsTrue(o.Equals(new Lzma2BlockSize(2000)));
			Assert.IsFalse(o.Equals(new Lzma2BlockSize(9000)));
			Assert.IsTrue(new Lzma2BlockSize(2000).Equals(new Lzma2BlockSize(2000)));
			Assert.IsFalse(new Lzma2BlockSize(8000).Equals(new Lzma2BlockSize(2000)));
			Assert.IsFalse(o.Equals(null));
			Assert.IsFalse(o.Equals(new LzmaMatchFindPasses(88)));

			// HashCode
			int hash = new Lzma2BlockSize(88888888L).GetHashCode();
			Assert.AreNotEqual(0, hash);

			// ToString
			string s = new Lzma2BlockSize(99999).ToString();
			Assert.IsFalse(String.IsNullOrEmpty(s));
		}

		[TestMethod(), TestCategory("Xz")]
		public void Xz_Lzma2MaximumThreads()
		{
			// Constructors
			var threads = new Lzma2MaximumThreads(5);
			Assert.AreEqual(5, threads);

			try { threads = new Lzma2MaximumThreads(-1); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			// Implicit conversion
			threads = 8;
			Assert.AreEqual(8, threads);

			// Equality
			Assert.IsTrue(new Lzma2MaximumThreads(8) == threads);
			Assert.IsFalse(new Lzma2MaximumThreads(1) == threads);
			Assert.IsTrue(new Lzma2MaximumThreads(3) != threads);
			Assert.IsFalse(new Lzma2MaximumThreads(8) != threads);

			object o = new Lzma2MaximumThreads(2);
			Assert.IsTrue(o.Equals(new Lzma2MaximumThreads(2)));
			Assert.IsFalse(o.Equals(new Lzma2MaximumThreads(9)));
			Assert.IsTrue(new Lzma2MaximumThreads(2).Equals(new Lzma2MaximumThreads(2)));
			Assert.IsFalse(new Lzma2MaximumThreads(8).Equals(new Lzma2MaximumThreads(2)));
			Assert.IsFalse(o.Equals(null));
			Assert.IsFalse(o.Equals(new LzmaMatchFindPasses(88)));

			// HashCode
			int hash = new Lzma2MaximumThreads(8).GetHashCode();
			Assert.AreNotEqual(0, hash);

			// ToString
			string s = new Lzma2MaximumThreads(9).ToString();
			Assert.IsFalse(String.IsNullOrEmpty(s));
		}

		[TestMethod(), TestCategory("Xz")]
		public void Xz_Lzma2ThreadsPerBlock()
		{
			// Constructors
			var threads = new Lzma2ThreadsPerBlock(5);
			Assert.AreEqual(5, threads);

			try { threads = new Lzma2ThreadsPerBlock(-1); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			// Implicit conversion
			threads = 8;
			Assert.AreEqual(8, threads);

			// Equality
			Assert.IsTrue(new Lzma2ThreadsPerBlock(8) == threads);
			Assert.IsFalse(new Lzma2ThreadsPerBlock(1) == threads);
			Assert.IsTrue(new Lzma2ThreadsPerBlock(3) != threads);
			Assert.IsFalse(new Lzma2ThreadsPerBlock(8) != threads);

			object o = new Lzma2ThreadsPerBlock(2);
			Assert.IsTrue(o.Equals(new Lzma2ThreadsPerBlock(2)));
			Assert.IsFalse(o.Equals(new Lzma2ThreadsPerBlock(9)));
			Assert.IsTrue(new Lzma2ThreadsPerBlock(2).Equals(new Lzma2ThreadsPerBlock(2)));
			Assert.IsFalse(new Lzma2ThreadsPerBlock(8).Equals(new Lzma2ThreadsPerBlock(2)));
			Assert.IsFalse(o.Equals(null));
			Assert.IsFalse(o.Equals(new LzmaMatchFindPasses(88)));

			// HashCode
			int hash = new Lzma2ThreadsPerBlock(8).GetHashCode();
			Assert.AreNotEqual(0, hash);

			// ToString
			string s = new Lzma2ThreadsPerBlock(9).ToString();
			Assert.IsFalse(String.IsNullOrEmpty(s));
		}

		[TestMethod(), TestCategory("Xz")]
		public void Xz_Encoder()
		{
			XzEncoder encoder = new XzEncoder();

			// Check the default values
			Assert.AreEqual(Lzma2BlockSize.Disabled, encoder.BlockSize);
			Assert.AreEqual(LzmaCompressionLevel.Default, encoder.CompressionLevel);
			Assert.AreEqual(LzmaCompressionMode.Default, encoder.CompressionMode);
			Assert.AreEqual(LzmaDictionarySize.Default, encoder.DictionarySize);
			Assert.AreEqual(LzmaFastBytes.Default, encoder.FastBytes);
			Assert.AreEqual(LzmaHashBytes.Default, encoder.HashBytes);
			Assert.AreEqual(LzmaLiteralContextBits.Default, encoder.LiteralContextBits);
			Assert.AreEqual(LzmaLiteralPositionBits.Default, encoder.LiteralPositionBits);
			Assert.AreEqual(LzmaMatchFindMode.Default, encoder.MatchFindMode);
			Assert.AreEqual(LzmaMatchFindPasses.Default, encoder.MatchFindPasses);
			Assert.AreEqual(Lzma2MaximumThreads.Default, encoder.MaximumThreads);
			Assert.AreEqual(LzmaPositionBits.Default, encoder.PositionBits);
			Assert.AreEqual(Lzma2ThreadsPerBlock.Default, encoder.ThreadsPerBlock);
			Assert.AreEqual(true, encoder.UseMultipleThreads);
			Assert.AreEqual(true, encoder.WriteEndMark);

			// Set and reset encoder parameters to exercise the property setters
			encoder.BlockSize = Int32.MaxValue;
			Assert.AreEqual(Int32.MaxValue, encoder.BlockSize);
			encoder.BlockSize = Lzma2BlockSize.Disabled;
			Assert.AreEqual(Lzma2BlockSize.Disabled, encoder.BlockSize);

			encoder.CompressionLevel = LzmaCompressionLevel.Fastest;
			Assert.AreEqual(LzmaCompressionLevel.Fastest, encoder.CompressionLevel);
			encoder.CompressionLevel = LzmaCompressionLevel.Default;
			Assert.AreEqual(LzmaCompressionLevel.Default, encoder.CompressionLevel);

			encoder.CompressionMode = LzmaCompressionMode.Fast;
			Assert.AreEqual(LzmaCompressionMode.Fast, encoder.CompressionMode);
			encoder.CompressionMode = LzmaCompressionMode.Default;
			Assert.AreEqual(LzmaCompressionMode.Default, encoder.CompressionMode);

			encoder.DictionarySize = LzmaDictionarySize.Maximum;
			Assert.AreEqual(LzmaDictionarySize.Maximum, encoder.DictionarySize);
			encoder.DictionarySize = LzmaDictionarySize.Default;
			Assert.AreEqual(LzmaDictionarySize.Default, encoder.DictionarySize);

			encoder.FastBytes = LzmaFastBytes.Maximum;
			Assert.AreEqual(LzmaFastBytes.Maximum, encoder.FastBytes);
			encoder.FastBytes = LzmaFastBytes.Default;
			Assert.AreEqual(LzmaFastBytes.Default, encoder.FastBytes);

			encoder.HashBytes = LzmaHashBytes.Maximum;
			Assert.AreEqual(LzmaHashBytes.Maximum, encoder.HashBytes);
			encoder.HashBytes = LzmaHashBytes.Default;
			Assert.AreEqual(LzmaHashBytes.Default, encoder.HashBytes);

			encoder.LiteralContextBits = LzmaLiteralContextBits.Maximum;
			Assert.AreEqual(LzmaLiteralContextBits.Maximum, encoder.LiteralContextBits);
			encoder.LiteralContextBits = LzmaLiteralContextBits.Default;
			Assert.AreEqual(LzmaLiteralContextBits.Default, encoder.LiteralContextBits);

			encoder.LiteralPositionBits = LzmaLiteralPositionBits.Maximum;
			Assert.AreEqual(LzmaLiteralPositionBits.Maximum, encoder.LiteralPositionBits);
			encoder.LiteralPositionBits = LzmaLiteralPositionBits.Default;
			Assert.AreEqual(LzmaLiteralPositionBits.Default, encoder.LiteralPositionBits);

			encoder.MatchFindMode = LzmaMatchFindMode.HashChain;
			Assert.AreEqual(LzmaMatchFindMode.HashChain, encoder.MatchFindMode);
			encoder.MatchFindMode = LzmaMatchFindMode.Default;
			Assert.AreEqual(LzmaMatchFindMode.Default, encoder.MatchFindMode);

			encoder.MatchFindPasses = LzmaMatchFindPasses.Maximum;
			Assert.AreEqual(LzmaMatchFindPasses.Maximum, encoder.MatchFindPasses);
			encoder.MatchFindPasses = LzmaMatchFindPasses.Default;
			Assert.AreEqual(LzmaMatchFindPasses.Default, encoder.MatchFindPasses);

			encoder.MaximumThreads = 256;
			Assert.AreEqual(256, encoder.MaximumThreads);
			encoder.MaximumThreads = Lzma2MaximumThreads.Default;
			Assert.AreEqual(Lzma2MaximumThreads.Default, encoder.MaximumThreads);

			encoder.PositionBits = LzmaPositionBits.Maximum;
			Assert.AreEqual(LzmaPositionBits.Maximum, encoder.PositionBits);
			encoder.PositionBits = LzmaPositionBits.Default;
			Assert.AreEqual(LzmaPositionBits.Default, encoder.PositionBits);

			encoder.ThreadsPerBlock = 10;
			Assert.AreEqual(10, encoder.ThreadsPerBlock);
			encoder.ThreadsPerBlock = Lzma2ThreadsPerBlock.Default;
			Assert.AreEqual(Lzma2ThreadsPerBlock.Default, encoder.ThreadsPerBlock);

			encoder.UseMultipleThreads = false;
			Assert.AreEqual(false, encoder.UseMultipleThreads);
			encoder.UseMultipleThreads = true;
			Assert.AreEqual(true, encoder.UseMultipleThreads);

			encoder.WriteEndMark = false;
			Assert.AreEqual(false, encoder.WriteEndMark);
			encoder.WriteEndMark = true;
			Assert.AreEqual(true, encoder.WriteEndMark);

			// Check all of the Encoder methods work and encode as expected
			byte[] expected, expectedstreamed, actual;

			// There are 2 expected results since when the length of the input
			// is known it's encoded into the compressed stream
			using (MemoryStream ms = new MemoryStream())
			{
				encoder.Encode(s_sampledata, ms);
				ms.Flush();
				expected = ms.ToArray();
			}

			using (MemoryStream ms = new MemoryStream())
			{
				encoder.Encode(new MemoryStream(s_sampledata), ms);
				ms.Flush();
				expectedstreamed = ms.ToArray();
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

			// Check actual encoding operations
			actual = encoder.Encode(s_sampledata);
			Assert.IsTrue(Enumerable.SequenceEqual(expected, actual));						// length is known

			actual = encoder.Encode(new MemoryStream(s_sampledata));
			Assert.IsTrue(Enumerable.SequenceEqual(expectedstreamed, actual));				// length is not known

			actual = encoder.Encode(s_sampledata, 0, s_sampledata.Length);					// length is known
			Assert.IsTrue(Enumerable.SequenceEqual(expected, actual));

			using (MemoryStream dest = new MemoryStream())
			{
				encoder.Encode(s_sampledata, dest);
				Assert.IsTrue(Enumerable.SequenceEqual(expected, dest.ToArray()));			// length is known
			}

			using (MemoryStream dest = new MemoryStream())
			{
				encoder.Encode(new MemoryStream(s_sampledata), dest);
				Assert.IsTrue(Enumerable.SequenceEqual(expectedstreamed, dest.ToArray()));	// length is not known
			}

			using (MemoryStream dest = new MemoryStream())
			{
				encoder.Encode(s_sampledata, 0, s_sampledata.Length, dest);
				Assert.IsTrue(Enumerable.SequenceEqual(expected, dest.ToArray()));			// length is known
			}
		}
	}
}
