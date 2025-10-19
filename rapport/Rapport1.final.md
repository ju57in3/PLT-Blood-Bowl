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

	
### 2.3 Conception logicielle: extension pour le rendu

### 2.4 Conception logicielle: extension pour le moteur de jeu

### 2.5 Ressources
