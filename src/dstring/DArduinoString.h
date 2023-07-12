#ifndef DSTRING_H
#define DSTRING_H

#include <Arduino.h>
#include <WString.h>

namespace std {
	class string {
		private:
			String str;

		public:
			string(void){

			};

			string(char *buff, size_t lenght) {
				char s[lenght+1];
				strcpy(s,buff);
				str=s;
			}

            string(const char *buff) : string((char*) buff,strlen(buff)) {}

			string(uint8_t *buff, size_t lenght) : string((char*)buff, lenght) {}

            char& operator[](size_t idx) {
                return str[idx];
            };

            bool operator==(const char* other) {
                return(str.equals(other));
            }

			const String& ArduinoString(void) {
				return(str);
			}

            const char* c_str(void) {
                return(str.c_str());
            }

			size_t size(void) {
				return(str.length());
			}
	};

}

#endif