# GP4C: Genetic Programming for Crawling


The quality of a Web search engine is influenced by several factors,
including coverage and the freshness of the content gathered by the web crawler.
Focusing particularly on freshness, one key challenge is to estimate
the likelihood of a previously crawled webpage being modified. 
Such estimates are used to define the order in which those pages should
be visited, and thus, can be exploited to reduce the cost of monitoring
crawled webpages for keeping updated versions.

GP4C is genetic programming based framework to generate score functions
that produce accurate rankings of pages regarding their probabilities of
having been modified. 


This code was used for the experiments in the following publications:

- Aécio S. R. Santos. [**Learning to Schedule Web Page Updates Using Genetic Programming.**](http://homepages.dcc.ufmg.br/~aeciosantos/static/publications/master-thesis.pdf) *Master Thesis at Universidade Federal de Minas Gerais.* Advisor: Nivio Ziviani. March 11, 2013.

- Aécio S. R. Santos, Nivio Ziviani, Jussara M. Almeida, Cristiano Carvalho, Edleno Silva de Moura, Altigran Soares da Silva. [**Learning to Schedule Webpage Updates Using Genetic Programming.**](http://homepages.dcc.ufmg.br/~aeciosantos/static/publications/spire13.pdf) *20th String Processing and Information Retrieval Symposium*, October 2013.

- Aécio S. R. Santos, Cristiano Carvalho, Jussara M. Almeida, Edleno Silva de Moura, Altigran Soares da Silva, Nivio Ziviani: [**A genetic programming framework to schedule webpage updates**](http://link.springer.com/article/10.1007%2Fs10791-014-9248-5). *Journal of Information Retrieval*, October 2014.
