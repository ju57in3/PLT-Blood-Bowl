# Projet Logiciel Transversal

---

#### Tom Compagnon,  Matteo Doffe,  Guilhem Boularan,  Justine Hazan

---

## 1 Objectif

### 1.1 Présentation générale
L'objectif de ce projet est le développement en C++ du jeu "Blood Bowl" avec une version des règles simplifiées.

### 1.2 Règles du jeu
Deux équipes s’affrontent pour marquer plus de touchdowns que l’adversaire en 16 tours en portant le ballon dans la zone d’en-but ennemie.

### 1.3 Ressources
Les bases des ressources du projet sont disponibles dans le dossier "res" sur GitHub. Nous avons choisi deux différentes races : les Humains et les Orcs, pour avoir différents types de joueurs sur le terrain.



## 2 Description et conception des états
Le jeu Blood Bowl est constitué d'une succession d'états représentant les différentes étapes d'une partie : la mise en place (Setup), l'engagement (Kickoff), le tour du joueur (PlayerTurn), la mi-temps (HalfTime) et la fin de partie (EndGame).
Chaque état définit un comportement spécifique du jeu : l'activation des personnages, les transitions entre les équipes et la réinitialisation de certaines variables.
Tous les états sont regroupés dans la classe principale BloodBowlGame, cette classe agit comme le contexte du système. Elle contient l'état courant (currentState) et les équipes.
Cela permet au jeu de fonctionner de manière que chaque état hérite d'une classe abstraite commune et implémente son propre comportement sans modifier la structure du jeu.
### 2.1 Description des états
#### 2.1.1 Setup
Il permet d'initialiser les équipes, il vérifie la validité du placement des personnages et prépare le terrain avant le coup d'envoi.
#### 2.1.2 Kickoff
Il gère la phase d'engagement avec la position de la balle, le choix du carré cible ainsi que d'éventuels rebonds.
#### 2.1.3 PlayerTurn
Il représente le tour actif d'un joueur et gère les mouvements, les actions et les touchdowns.
#### 2.1.4 HalfTime
Il marque une pause entre deux mi-temps et réinitialise certains paramètres (si besoin) et prépare la reprise du jeu.
#### 2.1.5 EndGame
Il signale la fin du match et affiche le score final. Il peut également proposer de relancer une partie.

### 2.2 Conception logicielle
#### 2.2.1 AbstractState
Cette classe a un rôle très important dans la hiérarchie : elle fournit l'interface commune à tous les états. Elle a un pointeur vers le jeu (BloodBowlGame *jeu) et ͏deux méthode͏s virtuelles: update() qui est ͏appelée à chaque passage pour appliquer la logique d’état; apply() qui peut être util͏isée pour initier ou vérifier la transition. Alors, chaque état hérite de Ab͏stractState et change ces méthod͏es selon ses besoins.
#### 2.2.2 Setup
Cette classe représente la phase d’initialisation du jeu. Elle prépare les deux équipes avant le début de la partie. Elle hérite de AbstractState, et redéfinit la méthode update() pour gérer la validation du placement des personnages, la préparation du terrain et la configuration initiale du match. La méthode apply() peut être utilisée pour vérifier que toutes les conditions sont remplies avant de passer à l’état suivant (par exemple : tous les joueurs sont positionnés, les équipes sont prêtes). Ainsi, Setup est responsable de la mise en place initiale et garantit que le jeu commence dans un état cohérent.
#### 2.2.3 Kickoff
Cette classe gère la phase d’engagement qui suit la mise en place. En héritant de AbstractState, elle implémente update() pour gérer le placement de la balle, la sélection de la case de lancement et les éventuels rebonds. La méthode apply() peut servir à initier la phase de tir ou à valider la fin de l’engagement avant de passer au tour de joueur. Kickoff sert de transition dynamique entre la préparation (Setup) et le déroulement du match (PlayerTurn), en assurant que la balle et les positions initiales sont correctement définies.
#### 2.2.4 PlayerTurn
Cette classe incarne le cœur du gameplay : elle correspond au tour d’une équipe. Comme les autres, elle hérite de AbstractState, et ses méthodes update() et apply() gèrent respectivement le déroulement du tour et les vérifications de fin de tour. Elle contient plusieurs attributs et méthodes pour suivre l’état du tour : isTurnOver(), getTurnNumber(), getTouchDown(), etc. C’est ici que sont contrôlées les actions des joueurs (mouvements, passes, blocages), ainsi que la logique de changement d’équipe. PlayerTurn se charge donc de la séquence principale du jeu, où les décisions des joueurs ont un impact direct sur la partie.
#### 2.2.5 Halftime
Cette classe correspond à la mi-temps. Elle hérite également de AbstractState et redéfinit update() pour appliquer les effets de pause entre deux périodes de jeu. Elle peut par exemple réinitialiser certains paramètres des équipes (positions, statuts des personnages, compteurs de relance) et afficher le score intermédiaire. La méthode apply() est utilisée pour préparer le jeu à reprendre la deuxième mi-temps. HalfTime agit donc comme un état de transition, assurant la continuité du match tout en maintenant la cohérence des données entre les deux périodes.
#### 2.2.6 EndGame
Cette classe marque la fin du match. Elle hérite de AbstractState et redéfinit update() pour gérer la conclusion de la partie : affichage des scores, détermination du vainqueur, et remise à zéro éventuelle des équipes. Elle possède un attribut restart (booléen) qui indique si une nouvelle partie doit être relancée. La méthode apply() peut être utilisée pour réinitialiser tous les personnages (par exemple, en les renvoyant sur le banc) si le joueur choisit de recommencer. EndGame clôt donc la séquence du jeu et permet soit de sortir, soit de redémarrer depuis l’état Setup.
#### 2.2.7 BloodBowlGame
Cette classe joue un rôle central : elle représente le contexte du jeu, dans lequel tous les états évoluent. Elle ne dérive pas de AbstractState, mais en contient une instance active via un pointeur currentState.
Elle stocke également :
- Les deux équipes (teamA, teamB) ;
- Les dimensions du terrain (width, height) ;
- La position de la balle (ballPosition) ;
- Une liste de tous les états disponibles (stateList).

BloodBowlGame gère les transitions entre états grâce à la méthode setCurrentState(). Elle agit donc comme le cœur du patron de conception State : au lieu d’utiliser de longs blocs de conditions, elle délègue le comportement à l’état actuellement actif. Chaque appel à update() ou apply() est transmis à currentState, ce qui rend le système flexible, extensible et conforme aux principes de l’orientation objet.
### 2.3 Conception logicielle: extension pour le rendu

### 2.4 Conception logicielle: extension pour le moteur de jeu

### 2.5 Ressources
