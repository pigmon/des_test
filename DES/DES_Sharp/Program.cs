using System;
using System.Collections;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DES_Sharp
{
    class Program
    {
        static void PrintBitArray(BitArray _in)
        {
            for (int i = _in.Length - 1; i >= 0; i--)
            {
                Console.Write(Convert.ToInt32(_in[i]));
            }
            Console.WriteLine();
        }

        static void PrintBitString(BitArray _in)
        {
            byte[] arr = new byte[_in.Length / 8];
            _in.CopyTo(arr, 0);

            System.Text.UTF8Encoding asciiEncoding = new System.Text.UTF8Encoding();
            string strCharacter = asciiEncoding.GetString(arr);
            Console.WriteLine(strCharacter);
        }

        static void Main(string[] args)
        {
            BinaryWriter bw = new BinaryWriter(new FileStream(@"D:\as.txt", FileMode.OpenOrCreate));

            Des obj = new Des("T&^9c=A`");
            string s = "To be or";
            BitArray cipher = obj.encrypt(s);
            //PrintBitString(cipher);
            byte[] arr = new byte[cipher.Length / 8];
            cipher.CopyTo(arr, 0);
            bw.Write(arr);
            bw.Close();

            BitArray plain = obj.decrypt(cipher);
            //PrintBitString(plain);
        }
    }
}
