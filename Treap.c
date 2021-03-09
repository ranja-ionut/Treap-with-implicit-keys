#include <stdio.h>
#include <time.h>
#include <stdlib.h>
typedef int Type;


// Treap structure
typedef struct sequence {
	Type item;
	int priority;
	struct sequence *left;
	struct sequence *right;
	int nodeCount;
} *Sequence;

Sequence* split(Sequence seq, int index);
Sequence concat(Sequence seq1, Sequence seq2);
int size(Sequence seq);
Type lookup(Sequence seq, int index);

/*
	Functia myAlloc creeaza un vector de pointeri 
	care retin toti pointerii intorsi de functia
	malloc pentru a putea usura procesul de elibarea
	a memoriei parcurgand vecotrul de pointeri si
	dandu-le free() fiecarui pointer in parte.
*/
void *myMalloc(size_t size,int clear, size_t *ptrs_count){
	static void **ptrs=NULL;
	static size_t ptrs_size=0;
	static size_t ptrs_cap=0;
	if(clear){
		*ptrs_count=ptrs_size;
		return ptrs;
	}
	if(ptrs_size+1>ptrs_cap){
		size_t new_cap=ptrs_size==0 ? 1 : 2*ptrs_size;
		ptrs=realloc(ptrs,sizeof(*ptrs)*new_cap);
		ptrs_cap=new_cap;
	}
	void *ptr=malloc(size);
	ptrs[ptrs_size++]=ptr;
	return ptr;
}
/*
	Se va apela myMalloc cu clear setat pe 0,
	insemnand adaugarea pointerului nou alocat. 
*/
void *gcMalloc(size_t size){
	return myMalloc(size,0,NULL);
}
/*
	Se va parcurge tot vectorul de pointeri alocati
	prin apelul myMalloc cu clear setat pe 1 care va 
	intoarce vectorul si dimenisunea sa; apoi se va 
	efectua comanda free() pe fiecare pointer in parte.
*/
void gc(){
	size_t ptrs_count;
	void **ptrs=(void**)myMalloc(0,1,&ptrs_count);
	for(size_t i=0;i<ptrs_count;i++){
		free(ptrs[i]);
	}
	free(ptrs);
}
/*
	Functia init() initializeaza un nou element
	de tip Sequence si il intoarce.
	Important este faptul ca fiecare nod are o prioritate
	aleasa aleatoriu pentru a pastra o structura de arbore echilibrat.
*/
Sequence init(){
	Sequence seq=gcMalloc(sizeof(*seq));
	seq->item=-1;
	srand(time(0));
	seq->priority=rand();
	seq->nodeCount=1;
	seq->left=NULL;
	seq->right=NULL;
	return seq;
}
/*
	Functia reCount() actualizeaza numarul de noduri
	al elementului seq prin formula numar noduri seq = 
	numar noduri arbore stang + numar noduri arbore drept
	+ el insusi.
*/
void reCount(Sequence seq){
	if(seq!=NULL)
		seq->nodeCount=size(seq->left)+size(seq->right)+1;
}
/*
	Functia insert va insera un nou element Sequence la 
	pozitia specificata de index.
	Principiul de functionare este ruperea secventei in
	doua parti, elementele de la 0 la index-1 si de la index
	la size(seq)-1, si concatenarea noului element la cea de-a
	doua secventa, apoi concatenarea rezultatului la prima parte.
*/
Sequence insert(Sequence seq, Type item, int index){
	if(seq!=NULL)
		if(size(seq)-index<0||size(seq)+index<size(seq))
			return seq;
	Sequence *seqv=NULL;
	seqv=split(seq,index-1);
	Sequence newSeq=init();
	newSeq->item=item;
	if(seq!=NULL)
	{
		Sequence ret=concat(newSeq,seqv[1]);
		ret=concat(seqv[0],ret);
		return ret;
	}
	return newSeq;
}
/*
	Functia delete va imparti secventa initiala in doua parti,
	de la 0 la index-1 si de la index la size(seq)-1. Apoi va imparti rezultatul
	de la index la size(seq)-1 in alte doua parti, de la index la index+1 si
	de la index +1 la siez(seq)-1, ramanand exclus din secventa doar elementul
	de la pozitia index. Se va realiza eliminarea acestui element prin concatenarea
	rezultatelor anterioare in ordine inversa fara introducerea acestuia in procesele
	de concatenare.
*/
Sequence delete(Sequence seq, int index){
	if(size(seq)-index<0||size(seq)+index<size(seq))
		return seq;
	Sequence *seqv1, *seqv2;
	seqv1=split(seq,index-1);
	seqv2=split(seqv1[1],0);
	return concat(seqv1[0],seqv2[1]);
}
/*
	Functia lookup se va folosi de numarul de noduri ai copiilor 
	radacinii si facand anumite calcule pentru determinarea pozitiei
	nodului cu index-ul dat, va intoarce valoarea acestuia daca exista.
	Nodurile respecta proprietatea de arbore binar din punct de vedere
	al indecsilor, acest lucru ajuta la gasirea pozitiei prin aplicarea
	formulelor de cautare.
*/
Type lookup(Sequence seq, int index){
	if(seq==NULL||size(seq)-index<0||size(seq)+index<size(seq))
		return -1;
	if(index==size(seq->left))
		return seq->item;
	else
		if(index<size(seq->left))
			return lookup(seq->left,index);
		else
			return lookup(seq->right,index-size(seq->left)-1);
}
/*
	Functia set realizeaza acelasi lucru cu functia delete,
	dar in loc sa scape de elemntul de pe pozitia index, functia
	doar ii va modifica valoarea si va readuce secventa la forma initiala.
*/
Sequence set(Sequence seq, Type item, int index){
	if(seq==NULL)
		return NULL;
	if(size(seq)-index<0||size(seq)+index<size(seq))
		return seq;
	Sequence *seqv1,*seqv2;
	seqv1=split(seq,index-1);
	seqv2=split(seqv1[1],0);
	seqv2[0]->item=item;
	Sequence ret=concat(seqv2[0],seqv2[1]);
	ret=concat(seqv1[0],ret);
	return ret;
}
/*
	Functia size intoarce numarul de noduri al unui nod radacina.
*/
int size(Sequence seq){
	if(seq==NULL)
		return 0;
	return seq->nodeCount;
}
/*
	Functia split realizeaza impartirea unei secventa in bucati,
	intr-un arbore stang si unul drept, lucrand cu arborele in care se
	afla index-ul. Ideea de la baza split-ului este faptul ca a cauta 
	index-ul la care sa se face split intr-o secventa data este echivalent
	cu a cauta pe copiii stangi si drepti dintr-un arbore cu 3 noduri, ajungandu-se
	in final la o structura de forma:
									index
								   /     \
								 NULL    NULL
	,moment in care se poate realiza parcurgerea inapoi spre radacina pentru
	a determina cele doua bucati in care va fi rupt Treap-ul.
*/
Sequence* split(Sequence seq, int index){
	if(seq==NULL)
		return NULL;
	else
	{
		if(size(seq->left)<=index)
		{
			Sequence *split_right;
			split_right=split(seq->right,index-size(seq->left)-1);
			if(split_right==NULL)
			{
				split_right=gcMalloc(2*sizeof(Sequence));
				split_right[0]=split_right[1]=NULL;
			}
			seq->right=split_right[0];
			split_right[0]=seq;
			reCount(seq);
			return split_right;
		}
		else
		{
			Sequence *split_left;
			split_left=split(seq->left,index);
			if(split_left==NULL)
			{
				split_left=gcMalloc(2*sizeof(Sequence));
				split_left[0]=split_left[1]=NULL;
			}
			seq->left=split_left[1];
			split_left[1]=seq;
			reCount(seq);
			return split_left;
		}
	}
}
/*
	Functia de concatenare foloseste prioritatile determinate
	aleatoriu in functia init() pentru a imbina Treap-ul initial
	sau a reimbina noul Treap asupra carora s-au efectuat operatii
	care ii modifica forma, functia asigurandu-se de faptul ca 
	Treap-ul rezultat este unul echilibrat.

*/
Sequence concat(Sequence seq1, Sequence seq2){
	if(seq1==NULL||seq2==NULL)
		return seq1 ? seq1:seq2;
	else
		if(seq1->priority>seq2->priority)
		{
			seq1->right=concat(seq1->right,seq2);
			reCount(seq1);
			return seq1;
		}
		else
		{
			seq2->left=concat(seq1,seq2->left);
			reCount(seq2);
			return seq2;
		}
}
/*
	Functie de parcurgete in inordine a Treap-ului 
	pentru afisarea valorilor sale.
*/
void treapPrint(Sequence seq) {
	if (seq == NULL) {
		return;
	}

	treapPrint(seq->left);
	printf("%3i ", seq->item);
	treapPrint(seq->right);
}
int main(){
	/*
		Secventa de cod pentru testarea functionalitatii programului.
	*/
	Sequence seq=NULL;
	printf("Secventa initiala: \nNULL\n\n");

	seq=insert(seq,10,0);
	seq=insert(seq,2,0);
	seq=insert(seq,101,0);
	seq=insert(seq,43,0);
	seq=insert(seq,-23,0);
	seq=insert(seq,-21,0);
	seq=insert(seq,0,0);
	seq=insert(seq,199,0);
	seq=insert(seq,32,0);
	seq=insert(seq,100,0);
	seq=insert(seq,-5,0);
	seq=insert(seq,44,0);
	seq=insert(seq,33,100);
	seq=insert(seq,-9,-1);
	seq=insert(seq,-4,-4);
	seq=insert(seq,-54,0);
	seq=insert(seq,23,0);
	printf("Secventa dupa inserari: \n");
	treapPrint(seq);
	printf("\n\n");
	
	printf("Secventa dupa stergerea elementului de la index = 0:\n");
	seq=delete(seq,0);
	treapPrint(seq);
	printf("\n\n");
	printf("Secventa dupa stergerea elementului de la index = 4:\n");
	seq=delete(seq,4);
	treapPrint(seq);
	printf("\n\n");
	printf("Secventa dupa stergerea elementului de la index = 100:\n");
	seq=delete(seq,100);
	treapPrint(seq);
	printf("\n\n");
	printf("Secventa dupa stergerea elementului de la index = -100:\n");
	seq=delete(seq,-100);
	treapPrint(seq);
	printf("\n\n");
	printf("Secventa dupa stergerea elementului de la index = 3:\n");
	seq=delete(seq,3);
	treapPrint(seq);
	printf("\n\n");

	printf("Valoarea elementului de la index = 4: %i\n\n",lookup(seq,4));
	printf("Valoarea elementului de la index = 8: %i\n\n",lookup(seq,8));
	printf("Valoarea elementului de la index = 0: %i\n\n",lookup(seq,0));

	printf("Secventa dupa inlocuirea valorii elementului de la index = 4 cu 13:\n");
	seq=set(seq,13,4);
	treapPrint(seq);
	printf("\n\n");

	printf("Secventa dupa inlocuirea valorii elementului de la index = 8 cu 72:\n");
	seq=set(seq,72,8);
	treapPrint(seq);
	printf("\n\n");

	printf("Secventa dupa inlocuirea valorii elementului de la index = 0 cu 54:\n");
	seq=set(seq,54,0);
	treapPrint(seq);
	printf("\n\n");

	printf("Cele 2 subsecvente dupa apelarea lui split cu index=size(seq)/2:\n");
	Sequence *sub=split(seq,size(seq)/2);
	treapPrint(sub[0]);
	printf("\n");
	treapPrint(sub[1]);
	printf("\n\n");
	seq=concat(sub[0],sub[1]);
	sub=NULL;

	printf("Secventa reconcatenata:\n");
	treapPrint(seq);
	printf("\n\n");
	printf("Marimea secventei dupa toate operatiile:\n %i\n",size(seq));
	//apelarea lui gc() asigura eliberarea intregii memorii alocate
	gc();
	return 0;
}