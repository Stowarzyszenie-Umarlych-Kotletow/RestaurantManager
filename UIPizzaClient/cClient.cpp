#include "cClient.h"

//inicjacja klasy cClient przy pomocy makro wxwidgets
wxIMPLEMENT_APP(cClient);

cClient::cClient()
{

}

cClient::~cClient()
{

}

bool cClient::OnInit()
{
	// tworzenie nowego obiektu ramki UI admina
	window = new cMainClient();
	// wyœwietlenie ramki 
	window->Show();
	return true;
}
