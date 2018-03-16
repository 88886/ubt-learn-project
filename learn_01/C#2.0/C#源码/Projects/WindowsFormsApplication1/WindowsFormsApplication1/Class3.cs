using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.Serialization.Json;
using System.IO;
using System.Runtime.Serialization;

namespace WindowsFormsApplication1
{
    public static class JSON
    {

        public static T parse<T>(string jsonString)
        {
            using (var ms = new MemoryStream(Encoding.UTF8.GetBytes(jsonString)))
            {
                return (T)new DataContractJsonSerializer(typeof(T)).ReadObject(ms);
            }
        }

        public static string stringify(object jsonObject)
        {
            using (var ms = new MemoryStream())
            {
                new DataContractJsonSerializer(jsonObject.GetType()).WriteObject(ms, jsonObject);
                return Encoding.UTF8.GetString(ms.ToArray());
            }
        }
    }

    [DataContract]
    public class Person
    {
        [DataMember(Order = 0, IsRequired = true)]
        public string Name { get; set; }

        [DataMember(Order = 1)]
        public int Age { get; set; }

        [DataMember(Order = 2)]
        public bool Alive { get; set; }

        [DataMember(Order = 3)]
        public string[] FavoriteFilms { get; set; }

        [DataMember(Order = 4)]
        public Person Child { get; set; }
    }

    
}
