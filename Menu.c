#include "General.h"
#include <string.h>
#include "Menu.h"


//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                METODOS PUBLICOS
//********************************************************************************************************************    
//********************************************************************************************************************    

//Función que actualiza la referencia al item inmediatamente superior en la jerarquía de arbol.
//Si se llegó al nodo raiz o padre, se devolverá FALSE para indicar que estamos en la cima.
ITEM_MENU* Move_Item_UP(ITEM_MENU* item, ITEM_MENU* itemPadre)
{
    if(item->Padre != itemPadre)
    {
        item = item->Padre;
    }
    
    return item;
    
}

ITEM_MENU* Move_Item_DOWN(ITEM_MENU* item)
{
    if(item->Hijo != NULL)
    {
        item = item->Hijo;
    }
    return item;
}

ITEM_MENU* Move_Item_IZQ(ITEM_MENU* item)
{
    if(item->Hermano_Izq != NULL)
    {
        item = item->Hermano_Izq;
    }
    return item;
}

ITEM_MENU* Move_Item_DCH(ITEM_MENU* item)
{
    if(item->Hermano_Dch != NULL)
    {
        item = item->Hermano_Dch;
    }
    return item;
}

void Ini_Item_Menu(ITEM_MENU* item, ITEM_MENU* padre, ITEM_MENU* hijo, ITEM_MENU* herm_izq, ITEM_MENU* herm_dch, char* texto, void (*fptr)(void))
{
    item->Padre = padre;
    item->Hijo = hijo;
    item->Hermano_Izq = herm_izq;
    item->Hermano_Dch = herm_dch;
    strcpy((char*)item->Texto, texto);
    item->funcion = fptr;
}



