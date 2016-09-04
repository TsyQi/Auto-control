#ifndef BITREE_H
#define BITREE_H
#include <iostream>
/****************��ʽ�洢****************/
typedef struct BinaryTreeNode {
	char data;//����
	struct BinaryTreeNode* m_Lsub, *m_Rsub;//������ָ��
}BinNode, *BinTree;
/****************�������Ĵ���****************/
//���������д���������
int CreateBiTree(BinTree &T)
{
	char data;
	//�������������������н���ֵ��һ���ַ�������#����ʾ����
	scanf("%c", &data);
	if (data == '#')
	{
		T = NULL;
	}
	else
	{
		T = (BinTree)malloc(sizeof(BinNode));
		//���ɸ��ڵ�
		T->data = data;
		//������������
		CreateBiTree(T->m_Lsub);
		CreateBiTree(T->m_Rsub);
	}
	return 0;
}
/****************����������****************/
//�ݹ�
void Visit(BinTree T)
{
	if (T->data != '#')
	{
		printf("%c", T->data);
	}
}
//�������
void PreOrder(BinTree T)
{
	if (T != NULL)
	{
		//���ʸ��ڵ�
		Visit(T);
		//�������ӽڵ�
		PreOrder(T->m_Lsub);
		//�������ӽڵ�
		PreOrder(T->m_Rsub);
	}
}
//�������

//������������
int GetNodeNum(BinNode *pRoot)
{
	if (pRoot == NULL)//�ݹ����
		return 0;
	return GetNodeNum(pRoot->m_Lsub) + GetNodeNum(pRoot->m_Rsub) + 1;
}
//���������
int GetDepth(BinNode *pRoot)
{
	if (pRoot == NULL)
		return 0;
	int depthL = GetDepth(pRoot->m_Lsub);
	int depthR = GetDepth(pRoot->m_Rsub);
	return depthL > depthR ? (depthL + 1) : (depthR + 1);
}
//
#endif // !BITREE_H
