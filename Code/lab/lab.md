# Laboratoire

Équipe: 1
Membres: Marc-Olivier Fecteau (FECM0701), Gabriel Aubut (AUBG####)

## Partie 1 - Programmation concurentielle

### Exercice 1 - Accès concurrent

#### Question 1.1

Q:	Quelle variable est partagée entre les fils d’exécution?  
A:	la variable *somme_*.

#### Question 1.2

Q:	Identifiez la section critique.
A:	La section critique se situe dans la fonction *accum*, lorsqu'un entier *i* est ajouté à 
	*somme_* sans protection de donnée.

#### Question 1.3

Q:	Mettez en place un mécanisme de synchronisation pour contrôler l’accès à cette variable.  
A:	Trois méthodes sont possibles: *lock-unlock*, *lock_guard*, et *unique_lock*. *unique_lock* 
	a été choisie (voir ex1.cpp).

#### Question 1.4

Q:	Pourquoi y a-t-il une perte de performance?  
A:	Le temps supplémentaire pour protéger la variable *somme_* augmente considérablement le 
	temps d'exécution.

Q:	Proposez une solution.  
A:	N'utiliser qu'un seul thread. Alternativement, et plus efficacement, le programme pourrait 
	utiliser l'équation pour obtenir la sommation de nombres naturels consécutifs, tel que montré 
	ci-dessous:

```cpp
	void accum(int d, int f) {
		somme = (f - d) * (f - d + 1) / 2;
	}
```

En utilisant cette méthode, le temps d'exécution est TOUJOURS le même (*O(1)*).

### Exercice 2 - *Monitoring*

#### Question 2.1

Q:	Qu’est-ce qui cause la consommation abusive du processeur?  
A:	Le *thread thread_consommateur* lit continuellement la file de données, 
	alors que *thread_producteur* n'envoie pas continuellement des données.

#### Question 2.2

Q:	Ajoutez une pause entre les lectures. Est-ce que ça diminue la consommation?  
A:	Oui, la consommation initiale était de X%, et la consommation avec pauses est de Y%.

#### Question 2.3

Q:	Implémentez un mécanisme de synchronisation.  
A:	La solution proposée utilise un *boolean flag* nommé *is_prod_done_*, qui devient *VRAI* 
	lorsque le producteur a ajouté une donnée à la file. Le *consommateur* ne lie les données 
	de la file que si *is_prod_done_* est *VRAI*, et ne change la valeur de *is_prod_done_* 
	que lorsque la file est vidée.

#### Question 2.4 (facultative)

Q:	Implémentez une façon d'arrêter *consommateur* avec le *boolean flag* nommé *should_run_*, qui est 
	géré par *main*.  
A:	La solution proposée remplace la boucle infinie dans *consommateur* par une boucle conditionnelle 
	selon *should_run_*. Lorsque la tâche de *producteur* est terminée, et que *join()* a été appelée, 
	*main* change la valeur de *should_run_* pour *FAUX*, et *consommateur* devrait s'arrêter.