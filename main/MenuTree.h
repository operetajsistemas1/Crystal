/*
 * MenuTree.h
 *
 * Created: 11.06.2017 12:41:42
 *  Author: Andris
 */ 


#ifndef MENUTREE_H_
#define MENUTREE_H_

typedef struct tree_node tree_node;

struct tree_node {
    tree_node *parent; // points to parent object, root has no parent = NULL
    char *text; // text to be drawn
    char selected; // indicates if item is selected or not
    tree_node * child[6]; // points to all 5 childs, if any
};

void MENU_Draw();
void MENU_Down();
void MENU_In();
void MENU_Out();
void MENU_Up();

void MENU_Process(uint8_t);
void MENU_Print_Time(uint16_t);
void MENU_Status();
void MENU_Status_Header();
void MENU_Status_Header2();

#endif /* MENUTREE_H_ */