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

			string(uint8_t *buff, size_t lenght) : string((char*)buff, lenght) {}

			String& ArduinoString(void) {
				return(str);
			}

			size_t size(void) {
				return(str.length());
			}

			char &operator[](size_t idx) {
                return str[idx];
            };
	};

}

#endif