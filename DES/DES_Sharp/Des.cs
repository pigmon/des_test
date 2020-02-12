using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace DES_Sharp
{
    class Des
    {
        private BitArray m_key = new BitArray(64);
        private List<BitArray> m_sub_keys = new List<BitArray>();

        static void PrintBitArray(BitArray _in)
        {
            for (int i = _in.Length - 1; i >= 0; i--)
            {
                Console.Write(Convert.ToInt32(_in[i]));
            }
            Console.WriteLine();
        }

        public Des(string _key)
        {
            m_key = encode_key(_key);
            gen_sub_keys();
        }

        public BitArray encode_key(string _key)
        {
            Debug.Assert(_key.Length == 8, "[encode_key] Key Length must be 8.");

            BitArray bits = new BitArray(64);
            for (short i = 0; i < 8; ++i)
            {
                byte[] _key_i = { (byte)_key[i] };
                BitArray tmp = new BitArray(_key_i);

                for (int j = 0; j < 8; j++)
                {
                    bits[i * 8 + j] = tmp[j] & true;
                }
            }

            return bits;
        }

        private void left_shift_in_key_56(ref BitArray _key, int _shift)
        {
            Debug.Assert(_key.Length == 28, "[left_shift_in_key_56] Key Length must be 28.");

            BitArray tmp = new BitArray(_key);
            for (int i = 27; i >= 0; --i)
            {
                if (i - _shift < 0)
                    _key[i] = tmp[i - _shift + 28];
                else
                    _key[i] = tmp[i - _shift];
            }
        }

        private void gen_sub_keys()
        {
            BitArray real_key = new BitArray(56);
            BitArray left = new BitArray(28);
            BitArray right = new BitArray(28);
            BitArray key_48 = new BitArray(48);

            // 去掉奇偶标记位，将64位密钥变成56位
            for (int i = 0; i < 56; ++i)
                real_key[55 - i] = m_key[64 - Table.PC_1[i]];

            // 生成子密钥，保存在 subKeys[16] 中
            for (int round = 0; round < 16; ++round)
            {
                // 前28位与后28位
                for (int i = 28; i < 56; ++i)
                    left[i - 28] = real_key[i];
                for (int i = 0; i < 28; ++i)
                    right[i] = real_key[i];

                // 左移
                left_shift_in_key_56(ref left, Table.shiftBits[round]);
                left_shift_in_key_56(ref right, Table.shiftBits[round]);

                // 压缩置换，由56位得到48位子密钥
                for (int i = 28; i < 56; ++i)
                    real_key[i] = left[i - 28];
                for (int i = 0; i < 28; ++i)
                    real_key[i] = right[i];
                for (int i = 0; i < 48; ++i)
                    key_48[47 - i] = real_key[56 - Table.PC_2[i]];
                
                m_sub_keys.Add(new BitArray(key_48));
            }
        }

        public BitArray f(BitArray _r, BitArray _key)
        {
            BitArray ret = new BitArray(32);
            BitArray expand_r = new BitArray(48);

            // 1. E扩展置换
            for (int i = 0; i < 48; ++i)
                expand_r[47 - i] = _r[32 - Table.E[i]];

            // 2. 异或
            expand_r = expand_r.Xor(_key);

            // 3. S-Box 置换
            int x = 0;
            for (int i = 0; i < 48; i += 6)
            {
                int row = Convert.ToInt32(expand_r[47 - i]) * 2 + Convert.ToInt32(expand_r[47 - i - 5]);
                int col = Convert.ToInt32(expand_r[47 - i - 1]) * 8 + Convert.ToInt32(expand_r[47 - i - 2]) * 4 + 
                    Convert.ToInt32(expand_r[47 - i - 3]) * 2 + Convert.ToInt32(expand_r[47 - i - 4]);

                int num = Table.S_BOX[i / 6, row, col];
                BitArray binary = new BitArray(new int[] { num });
                ret[31 - x] = binary[3];
                ret[31 - x - 1] = binary[2];
                ret[31 - x - 2] = binary[1];
                ret[31 - x - 3] = binary[0];
                x += 4;
            }

            // 4. P置换
            BitArray tmp = new BitArray(ret);
            for (int i = 0; i < 32; ++i)
                ret[31 - i] = tmp[32 - Table.P[i]];

            return ret;
        }

        public BitArray encrypt(BitArray _plain)
        {
            Debug.Assert(_plain.Length == 64, "[encrypt] input _plain length must be 64.");

            BitArray cipher = new BitArray(64);
            BitArray bits = new BitArray(64);
            BitArray left = new BitArray(32);
            BitArray right = new BitArray(32);
            BitArray tmp = new BitArray(32);

            // 1. IP置换
            for (int i = 0; i < 64; ++i)
                bits[63 - i] = _plain[64 - Table.IP[i]];

            // 2. 得到 L 和 R
            for (int i = 32; i < 64; ++i)
                left[i - 32] = bits[i];
            for (int i = 0; i < 32; ++i)
                right[i] = bits[i];

            // 3. 16 轮迭代
            for (int round = 0; round < 16; ++round)
            {
                tmp = right;
                right = left.Xor(f(right, m_sub_keys[round]));
                left = tmp;
            }

            // 4. 迭代最后组合，R16L16
            for (int i = 0; i < 32; ++i)
                cipher[i] = left[i];
            for (int i = 32; i < 64; ++i)
                cipher[i] = right[i - 32];

            // 5. IP-1 置换
            bits = new BitArray(cipher);         
            for (int i = 0; i < 64; ++i)
                cipher[63 - i] = bits[64 - Table.IP_1[i]];

            return cipher;
        }

        public BitArray encrypt(string _s)
        {
            Debug.Assert(_s.Length == 8, "[encrypt] input _s length must be 8.");
            BitArray input = encode_key(_s);
            return encrypt(input);
        }

        public BitArray decrypt(BitArray _cipher)
        {
            Debug.Assert(_cipher.Length == 64, "[decrypt] input _cipher length must be 64.");

            BitArray plain = new BitArray(64);
            BitArray bits = new BitArray(64);
            BitArray left = new BitArray(32);
            BitArray right = new BitArray(32);
            BitArray tmp = new BitArray(32);

            // 1. IP置换
            for (int i = 0; i < 64; ++i)
                bits[63 - i] = _cipher[64 - Table.IP[i]];

            // 2. 得到 L 和 R
            for (int i = 32; i < 64; ++i)
                left[i - 32] = bits[i];
            for (int i = 0; i < 32; ++i)
                right[i] = bits[i];

            // 3. 16 轮迭代，sub key顺序要反过来
            for (int round = 0; round < 16; ++round)
            {
                tmp = right;
                right = left.Xor(f(right, m_sub_keys[15 - round]));
                left = tmp;
            }

            // 4. 迭代最后组合，R16L16
            for (int i = 0; i < 32; ++i)
                plain[i] = left[i];
            for (int i = 32; i < 64; ++i)
                plain[i] = right[i - 32];

            // 5. IP-1 置换
            bits = new BitArray(plain);
            for (int i = 0; i < 64; ++i)
                plain[63 - i] = bits[64 - Table.IP_1[i]];

            return plain;
        }

    } // class
} // namespace
