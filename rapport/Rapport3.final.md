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

### 2.3 Ressources
![Diagramme des états](res/diaState.png)
Le diagramme des états "diaState.png" se trouve dans le dossier "res" dans la branche "StateMachine".

## 3 Rendu: Stratégie et Conception
Le rendu correspond à la partie du projet qui gère l’aspect visuel du jeu *Blood Bowl*. Son rôle est de transformer les informations issues du moteur de jeu (le paquetage `state`) en une représentation graphique cohérente et fluide à l’écran.

### 3.1 Stratégie de rendu d'un état
Chaque état logique du jeu (défini dans `state::AbstractState` et ses sous-classes telles que `Setup`, `Kickoff`, `PlayerTurn`, etc.) possède une **représentation graphique** associée. Lorsqu’un état change dans le moteur, le module de rendu met à jour les éléments visuels correspondants.  
Ce fonctionnement repose sur trois étapes principales :

1. Le moteur (`BloodBowlGame`) conserve l’état courant du jeu.
2. Le module de rendu (`render`) lit cet état pour extraire les informations nécessaires à l’affichage (position des joueurs, balle, score…).
3. Les données sont affichées à l’écran à l’aide de la librairie **SFML 2.0** (`sf::RenderWindow`, `sf::Sprite`, `sf::Text`, etc.).

Dans la version **client/serveur**, cette même logique sera conservée :
- le client affichera la dernière scène reçue du serveur,
- et en cas de latence, un affichage temporaire (par exemple une animation de chargement) sera utilisé jusqu’à la réception du nouvel état.

Cette stratégie permet d’éviter les décalages visuels dus aux retards réseau.

Le rendu repose sur une **approche par scène**. Chaque phase du jeu (menu, options, tour du joueur A, tour du joueur B) correspond à une scène spécifique identifiée par un identifiant `SceneId`. Chaque scène contient un ensemble d’éléments graphiques (sprites, images de fond, texte) regroupés dans un objet `SceneData`.  
Cette organisation permet :
- de séparer la logique du jeu et son affichage,
- de changer instantanément de vue lors d’un changement d’état,
- et d’ajouter facilement de nouvelles scènes (par exemple un écran de fin de partie).

### 3.2 Conception logicielle
La conception logicielle du module `render` repose sur deux classes principales :
- `Scene` : classe centrale du rendu, responsable du dessin et de la gestion de la fenêtre.
- `SceneData` : classe contenant toutes les ressources visuelles (sprites, textures, plateau, etc.).

Leur dépendance principale est la classe `state::BloodBowlGame`, qui fournit les informations nécessaires à l’affichage de l’état courant du jeu.
#### 3.2.1 Classe `Scene`

Cette classe est responsable de la **création et du rendu complet d’une scène**.  
Elle contient :
- un attribut `sf::RenderWindow` représentant la fenêtre d’affichage,
- un identifiant d’état `SceneId` (`MENU`, `PLAYER_A`, `PLAYER_B`, etc.),
- une référence vers le jeu (`std::shared_ptr<state::BloodBowlGame>`),
- et une instance de `SceneData` contenant les ressources graphiques à afficher.

- `init()` : initialise la scène à partir des données du jeu.
- `drawScene()` : redessine l’ensemble des éléments visuels à l’écran.

La classe `Scene` fait le lien entre la logique du jeu (`state`) et le rendu graphique (`SFML`), en traduisant les données du modèle en objets visuels (`sf::Sprite`, `sf::Text`, etc.).

#### 3.2.2 Classe `SceneData`

`SceneData` regroupe toutes les **ressources visuelles** nécessaires à une scène :
- `playersSprites` : liste des sprites représentant les personnages,
- `ballSprite` : sprite de la balle,
- `board` : sprite du plateau de jeu.

- `init()` : charge les textures et initialise les positions des éléments.
- `draw()` : affiche les éléments à l’écran à l’aide de la fenêtre ou d’un `sf::RenderTarget`.

La relation entre `Scene` et `SceneData` est une **composition**, garantissant qu’une scène possède toujours ses propres ressources graphiques.

#### 3.2.3 Énumération `SceneId`

L’énumération `SceneId` définit les différents types de scènes affichables :
- `MENU`
- `OPTION`
- `PLAYER_A`
- `PLAYER_B`

Cette structure simplifie la gestion des transitions entre les scènes et facilite l’ajout de nouveaux écrans à l’avenir.

#### 3.2.4 Intégration avec SFML

Le module `render` exploite plusieurs classes de **SFML 2.0** :
- `sf::RenderWindow` pour la fenêtre d’affichage,
- `sf::Drawable` et `sf::Sprite` pour les éléments graphiques,
- `sf::Text` pour les informations à l’écran (score, tours, notifications).

Chaque mise à jour du jeu (mouvement, changement de tour, action spéciale) est immédiatement traduite par une mise à jour graphique correspondante.

### 3.3 Ressources
![Diagramme du rendu](res/diaRender.png)
Le diagramme du rendu "diaRender.png" se trouve dans le dossier "res" dans la branche "Master".

# 4. Règles de changement d’états et moteur de jeu

Cette section décrit les mécanismes qui permettent au jeu **Blood Bowl** de passer d’un état à un autre. Elle regroupe les événements déclencheurs, les éléments temporels, les règles internes, ainsi que la conception du moteur de jeu chargé de mettre en œuvre ces transitions.

Le **moteur de jeu (engine)** joue un rôle central : il garantit la cohérence du match. Il valide les commandes envoyées par le joueur, applique leurs effets et déclenche les changements d’état lorsque les conditions sont réunies.  
L’objectif est de s’assurer que chaque action respecte les règles du jeu et que l’état global du match reste cohérent à tout moment.


## 4.1 Horloge globale

Le jeu suit une **chronologie interne** qui ne dépend pas du temps réel mais de la séquence logique des actions.  
L’horloge globale définit l’ordre dans lequel les événements se produisent et garantit que certaines actions ne peuvent intervenir qu’à des moments précis.

Elle intervient principalement dans trois situations :

### **Début et fin d’un tour**
- Chaque équipe dispose d’une série d’actions tant que son tour n’est pas terminé.  
- L’horloge interne indique quand toutes les actions disponibles sont épuisées ou lorsqu’un événement comme un turnover survient (joueur au sol, passe ratée, interception, etc.).

### **Mi-temps et fin du match**
- L’horloge compte les tours joués par chaque équipe.  
- Arrivé au tour 8 de chaque mi-temps, on passe automatiquement à l’état `HalfTime`, puis `EndGame` après les deux périodes.

### **Résolution d’actions séquentielles**
Certaines commandes (comme un blocage) impliquent plusieurs sous-étapes :
- lancer de dés,
- choix du résultat,
- calcul des effets.

L’horloge sert à orchestrer ces étapes pour éviter toute action simultanée.

**Conclusion :** l’horloge globale fournit un cadre temporel garantissant que toutes les résolutions se produisent dans le bon ordre et sans chevauchement.


## 4.2 Changements extérieurs

Les **changements extérieurs** correspondent aux événements provoqués par le joueur (ou plus tard par un client distant en mode réseau).  
Ils déclenchent une commande que le moteur doit valider et appliquer.

**Exemples :**
- Déplacement d’un joueur → commande `Move`
- Blocage → commande `Block`
- Passe → commande `Pass`
- Engagement (Kickoff) → commande `Setup` ou `Kickoff`
- Fin du tour → transition vers l’équipe adverse

**Processus :**
1. Le joueur envoie une commande.  
2. Le moteur vérifie sa validité (portée, PM restants, règles de contact…).  
3. Si elle est valide, le moteur l’exécute et peut entraîner un changement d’état.


## 4.3 Changements autonomes

Les **changements autonomes** sont déclenchés par les règles internes du jeu, sans intervention directe du joueur.

**Exemples :**
- **Turnover automatique** : échec critique → passage immédiat au tour adverse.  
- **Touchdown** : entrée dans l’en-but → transition vers `Kickoff`.  
- **Fin du tour par épuisement des actions**.  
- **Fin de mi-temps** après 8 tours.  
- **KO / Blessure** : mise à jour automatique du statut.  
- **Interception** : provoque turnover → changement d’état.

Ces transitions garantissent le respect des règles de Blood Bowl même sans action du joueur.


## 4.4 Conception logicielle

Le moteur repose sur un ensemble de classes dédiées à l’exécution des commandes et à la gestion des transitions d’état.  
*(Le diagramme UML est censé illustrer cette architecture.)*

### **Rôle du moteur (`Engine`)**

La classe `Engine` contient :
- un historique des commandes (`commandHistory`),
- une référence vers l’état courant (`gameState`),
- une méthode clé `executeCommand()` appliquant la commande au modèle.

Le moteur :
- vérifie la faisabilité de la commande,
- valide qu’elle respecte les règles,
- applique les effets via la méthode polymorphe `Command::execute`.

### **Patron de conception : Command Pattern**

Chaque action est encapsulée dans une classe dérivée de `Command` :
- `Move`
- `Block`
- `Pass`
- `HalfTime`
- `Setup`
- etc.

**Avantages :**
- séparation claire logique / application,
- historique des actions,
- extension facile (une nouvelle commande = une nouvelle classe).

### **Résolution des actions complexes**
Ex. : `Block`
- jet de dés (`rollDice`),
- calcul du résultat (`BlockResult`),
- mise à jour des statuts des joueurs.

Le moteur orchestre les étapes selon l’horloge.


## 4.5 Extension pour l’IA

L’architecture basée sur le **Command Pattern** facilite l’ajout d’IA.

Processus :
1. L’IA lit `gameState`.  
2. Elle choisit une action.  
3. Elle génère une commande (`Move`, `Block`...).  
4. Elle l’envoie au moteur, comme un joueur humain.

Ainsi :
- l’IA ne modifie jamais directement l’état,
- elle respecte les mêmes règles,
- la difficulté varie seulement par la stratégie.

On peut donc avoir plusieurs IA cohérentes (simple, intermédiaire, avancée) sans toucher au moteur.


## 4.6 Extension pour la parallélisation

En architecture client/serveur ou multi-thread, le moteur doit accepter des commandes de plusieurs sources (UI, IA, réseau) tout en garantissant :

- cohérence de l’état,
- absence de concurrence,
- ordre d’exécution immuable.

### 1. **File de commandes (Command Queue)**
Chaque thread pousse les commandes dans une file sécurisée.  
Le moteur les lit dans un **thread unique**, assurant l’ordre.

### 2. **Verrouillage léger de l’état**
- `mutex` sur écriture,
- lecture parallèle autorisée pour l’affichage.

### 3. **Rendu et moteur séparés**
- Le rendu tourne dans son propre thread,
- Il lit une **copie contrôlée** de l’état,
- Le moteur reste seul à modifier l’état.


## Illustration 3 : Diagramme des classes du moteur de jeu  
![Diagramme de l'engine](res/diaEngine.png)

## 5 Intelligence Artificielle
Cette section décrit les stratégies et outils utilisés pour créer un joueur artificiel. 
L'objectif est de proposer des comportements qui utilisent exactement les mêmes commandes qu'un joueur humain : déplacement, blocage, passe, fin de tour, etc.
L'IA ne connaît pas plus d'informations que le joueur. Elle s'appuie uniquement sur l'état du jeu accessible via BloodBowlGame, et elle envoie ses ordres en utilisant les commandes du moteur, comme le ferait un utilisateur avec sa souris ou son clavier.

### 5.1 Stratégies
#### 5.1.1 Intelligence minimale
Pour ce premier niveau, l'objectif est de fournir une IA très simple, capable de jouer sans prise de décision complexe.
Cette IA doit pouvoir agir seule pendant son tour, tout en respectant les règles du jeu et les contraintes imposées par le moteur.
L'IA minimale fonctionne exclusivement sur le hasard. A chaque fois que c'est son tour, elle récupère la liste de toutes les commandes possibles à cet instant, sélectionne une de ces commandes au hasard et l'envoie au moteur.
La particularité importante est que l'IA ne fait aucun calcul.
Cette straégie offre une base fonctionnelle solide, suffisante pour valider le moteur et tester la cohérence du jeu.

"Le diagramme UML de l'IA est disponible sur Github dans la branche IA. Pour le moment l'IA n'est pas totalement fonctionnelle."

#### 5.1.2 Intelligence basée sur des heuristiques

#### 5.1.3 Intelligence basée sur les arbres de recherche