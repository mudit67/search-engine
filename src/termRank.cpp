#include "./../include/dataStructures.h"
#include <cmath>

extern InvertedIndex Index;
extern DocumentMapping DocMap;
extern termDocTable TfIdf;
/*Calculate Inverse Document Frequency (IDF):

Use the document frequencies to calculate the IDF for each term. The formula for
IDF is:

IDF(term) = log10(N / DF(term))
Where:

N is the total number of documents in your collection.
DF(term) is the document frequency of the term (the number of documents
containing the term)*/
double calcIDF(std::string term) {
  int N = DocMap.size();
  int DF = Index[term].size();
  return std::log10(N / DF);
}
/*
Calculate TF-IDF Score:
For each term in a document, calculate the TF-IDF score:
TF-IDF(term, document) = TF(term, document) * IDF(term)*/
void buildTfIDF() {
  // return TfIdf[{term, docId}] * calcIDF(term);
  for (const auto &[token, PostingArr] : Index) {
    for (const auto &Posts : PostingArr) {
      int Tf = Posts.positions.size();
      TfIdf[{token, Posts.docID}] = Tf * calcIDF(token);
    }
  }
}