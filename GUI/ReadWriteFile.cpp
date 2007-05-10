#include "ReadWriteFile.h"

#ifdef linux
void strreverse(char* begin, char* end)
{

	char aux;
	while(end>begin)
		aux=*end, *end--=*begin, *begin++=aux;
}

void itoa(int value, char* str, int base)
{

	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char* wstr=str;
	int sign;

	// Validate base
	if (base<2 || base>35){ *wstr='\0'; return; }

	// Take care of sign
	if ((sign=value) < 0) value = -value;

	// Conversion. Number is reversed.
	do *wstr++ = num[value%base]; while(value/=base);
	if(sign<0) *wstr++='-';
	*wstr='\0';

	// Reverse string
	strreverse(str,wstr-1);
}
#endif

void save_txt(string filename_out, string out)
{

    ofstream myfile (filename_out.c_str());

	if (myfile.is_open())
	{
        myfile <<out;
        myfile.close();
        cout<<"File "<< filename_out<<" written"<<endl;
	}else{
	    cout<<"Problem with file I/O: "<<filename_out<<endl;
	}

}

string load_txt(string filename_in)
{

    string line;
    string all_lines="";
    ifstream myfile (filename_in.c_str());

    if (myfile.is_open())
    {

        while (! myfile.eof())
        {
            getline (myfile,line);
            all_lines=all_lines +line +"\n";
        }
        myfile.close();
    }else{
    cout<<"Fichier non trouve: "<<filename_in<<endl;
    }

return all_lines;
}


string find_subs_between(string mess_in,string left, string right)
{
	//extracting posx:
	int pos;
	string Spos="";

	int loc_1 = mess_in.find( left, 0 );

   	if( loc_1 != string::npos ){
		int loc_2 = mess_in.find( right, loc_1+1 );

		if( loc_2 != string::npos ){
			Spos = mess_in.substr(loc_1 + left.size(), loc_2-loc_1 - left.size());
		}else{
		    cout<<"Can't find right element."<<endl;
		    }
	}else{
        cout<<"Can't find left element."<<endl;
    }

return Spos;
}
