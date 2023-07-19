# Fonctionnement des communications point à point bloquantes SEND/RECV

Ce deuxième exercise a pour but d'illustrer l'utilisation des communications bloquantes point à point.
Dans ce code, le processus MPI de rang 1 envoie un message au processus
de rang 2.

## Consignes

1. Prenez le temps de regarder le code dans sa globalité.

La récupération des rangs `rank` permet d'effectuer des actions différenciées pour
chaque processus par l'intermédiaire d'une simple condition `if`.
Ici, l'envoi est effectué par le rang 1 et la réception par le rang 2 grâce à la condition `if`.

2. Complétez la fonction qui permet au rang 1 d'envoyer la variable `message` de
 type entier (`integer`) au rang 2.

```python
comm.send(...)
```

3. Complétez maintenant la fonction qui permet de réceptionner le
message venant du rang 1.

```python
message = comm.recv(...)
```


5. Exécuter le code avec au moins 3 processus :

```bash
mpirun -np 4 python main.py
```

