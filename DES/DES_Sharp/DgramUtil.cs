using System;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.IO;
using System.Collections;

namespace DES_Sharp
{
    /* -----------------------------------------------------------------------------------------
     *  说明：
     用于处理结构体类型报文的加密（发送端）和解密（接收端）的工具集。
     
     Use Case:

            // 加密用的64位秘钥
            string key = "T&^9c=A`";

            // 建立测试用结构体，开头和结尾字段赋值以便简单测试
            UplinkDGram obj = new UplinkDGram();
            obj.m_gram_header = 0xAB1;
            obj.m_unuse2 = -255;

            /// Step 1:
            /// 发送端 - 将结构体对象加密，得到字节数组
            byte[] arr_encrypted = DgramUtil.EncryptStruct<UplinkDGram>(obj, key);

            /// Step 2:
            /// 接收端 - 将字节数组解密后，转换成结构体对象
            /// /// Step 2.1 : 建立用于接收数据的结构体对象
            UplinkDGram obj2 = new UplinkDGram();
            /// /// Step 2.2 : 关键步骤，调用解密并转换成结构体对象的函数
            DgramUtil.DecryptToStruct<UplinkDGram>(arr_encrypted, key, ref obj2);

            /// 输出验证
            Console.WriteLine("{0:x3}", obj2.m_gram_header);
            Console.WriteLine(obj2.m_unuse2);
     -----------------------------------------------------------------------------------------*/
    class DgramUtil
    {
        /// <summary>
        /// 将结构体转换成Byte数组，用于Socket发送
        /// </summary>
        /// <typeparam name="T">[in type] 结构体类型</typeparam>
        /// <param name="_struct">[in] 结构体对象</param>
        /// <returns>转换后的Byte数组</returns>
        public static byte[] StructToBytes<T>(T _struct)
        {
            int size = Marshal.SizeOf(typeof(T));
            byte[] arr_byte = new byte[size];
            IntPtr ptr_buffer = Marshal.AllocHGlobal(size);
            try
            {
                Marshal.StructureToPtr(_struct, ptr_buffer, true);
                Marshal.Copy(ptr_buffer, arr_byte, 0, size);
            }
            finally
            {
                Marshal.FreeHGlobal(ptr_buffer);
            }
            return arr_byte;
        }

        /// <summary>
        /// 将Byte数组转换成特定类型T的结构体，用于Socket接收
        /// </summary>
        /// <typeparam name="T">[return type] 结构体类型</typeparam>
        /// <param name="_arr_byte">[in] 将转换成结构体T的Byte数组</param>
        /// <returns>结构体T对象</returns>
        public static T BytesToStruct<T>(byte[] _arr_byte)
        {
            object ret_obj = null;
            int size = Marshal.SizeOf(typeof(T));
            IntPtr ptr_buffer = Marshal.AllocHGlobal(size);
            try
            {
                Marshal.Copy(_arr_byte, 0, ptr_buffer, size);
                ret_obj = Marshal.PtrToStructure(ptr_buffer, typeof(T));
            }
            finally
            {
                Marshal.FreeHGlobal(ptr_buffer);
            }
            return (T)ret_obj;
        }

        /// <summary>
        /// 发送数据时，将字节数组加密
        /// </summary>
        /// <param name="_bytes">[in] 字节数组</param>
        /// <param name="_key">[in] DES加密用的64位秘钥</param>
        /// <returns>加密后的Byte数组</returns>
        public static byte[] EncryptBytes(byte[] _bytes, string _key)
        {
            Debug.Assert(_key.Length == 8, "<EncryptBytes> key length must be 8.");
            Debug.Assert(_bytes.Length % 8 == 0, "<EncryptBytes> input byte array length must mod 8 == 0.");

            int batch_count = _bytes.Length / 8;

            byte[] ret = new byte[_bytes.Length];
            MemoryStream ms = new MemoryStream();
            Des obj = new Des(_key);

            for (int i = 0; i < batch_count; i++)
            {
                byte[] arr_plan_bytes = new byte[8];
                byte[] arr_encrypted_bytes = new byte[8];

                Array.Copy(_bytes, i * 8, arr_plan_bytes, 0, 8);
                BitArray bit_arr_plan = new BitArray(arr_plan_bytes);
                BitArray bit_arr_encrypted = obj.encrypt(bit_arr_plan);
                bit_arr_encrypted.CopyTo(arr_encrypted_bytes, 0);
                ms.Write(arr_encrypted_bytes, 0, 8);
            }

            ret = ms.ToArray();

            ms.Close();

            return ret;
        }

        /// <summary>
        /// 接收数据时，将字节数组解密
        /// </summary>
        /// <param name="_bytes">[in] 字节数组</param>
        /// <param name="_key">[in] DES加密用的64位秘钥</param>
        /// <returns>解密后的Byte数组</returns>
        public static byte[] DecryptBytes(byte[] _bytes, string _key)
        {
            Debug.Assert(_key.Length == 8, "<DecryptBytes> key length must be 8.");
            Debug.Assert(_bytes.Length % 8 == 0, "<DecryptBytes> input byte array length must mod 8 == 0.");

            int batch_count = _bytes.Length / 8;

            byte[] ret = new byte[_bytes.Length];
            MemoryStream ms = new MemoryStream();
            Des obj = new Des(_key);

            for (int i = 0; i < batch_count; i++)
            {
                byte[] arr_encrypted_bytes = new byte[8];
                byte[] arr_decrypted_bytes = new byte[8];

                Array.Copy(_bytes, i * 8, arr_encrypted_bytes, 0, 8);
                BitArray bit_arr_encrypted = new BitArray(arr_encrypted_bytes);
                BitArray bit_arr_decrypted = obj.decrypt(bit_arr_encrypted);
                bit_arr_decrypted.CopyTo(arr_decrypted_bytes, 0);
                ms.Write(arr_decrypted_bytes, 0, 8);
            }

            ret = ms.ToArray();

            ms.Close();

            return ret;
        }

        /// <summary>
        /// 将结构体转换成Byte数组，然后加密成字节数组，用于Socket发送
        /// </summary>
        /// <typeparam name="T">[in type] 结构体类型</typeparam>
        /// <param name="_struct">[in] 结构体对象</param>
        /// <param name="_key">[in] DES加密用的64位秘钥</param>
        /// <returns>加密后的Byte数组</returns>
        public static byte[] EncryptStruct<T>(T _struct, string _key)
        {
            Debug.Assert(_key.Length == 8, "<EncryptStruct> key length must be 8.");

            // Step 1. 结构体转字节数组
            byte[] arr_original = StructToBytes<T>(_struct);

            // Step 2. 字节流加密
            byte[] arr_encrypted = EncryptBytes(arr_original, _key);

            return arr_encrypted;
        }

        /// <summary>
        /// 将Byte数组解密，然后转换成类型T的结构体，用于Socket接收
        /// </summary>
        /// <typeparam name="T">[out type] 结构体类型</typeparam>
        /// <param name="_bytes">[in] 将转换成结构体T的Byte数组</param>
        /// <param name="_key">[in] DES加密用的64位秘钥</param>
        /// <param name="_out">[out] 结构体T对象</param>
        /// <returns>成功 true，否则 false</returns>
        public static void DecryptToStruct<T>(byte[] _bytes, string _key, ref T _out)
        {
            // 这里无法用 sizeof 判断结构体大小，暂时用转成字节数组的方式进行
            byte[] get_size = StructToBytes<T>(_out);
            Debug.Assert(get_size.Length % 8 == 0, "<DecryptToStruct> input struct size must mod 8 == 0.");
            Debug.Assert(_key.Length == 8, "<DecryptToStruct> key length must be 8.");
            Debug.Assert(_bytes.Length % 8 == 0, "<DecryptToStruct> input byte array length must mod 8 == 0.");

            // Step 1. 字节流解密
            byte[] arr_decrypted = DecryptBytes(_bytes, _key);
            // Step 2. 字节数组到结构体
            _out = BytesToStruct<T>(arr_decrypted);
        }

    }
}
