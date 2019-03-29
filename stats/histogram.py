import math
import matplotlib.pyplot as plt
import numpy as np

plt.rc('figure', figsize=(8, 6))

G = exec(open("x.py").read())

def f (dt):
    return 32*750/dt
def speeds (L):
    return L #list(map(f,L))

'''
def quad (v):
    return v*v
def qmean(L):
   return math.sqrt(sum(map(quad,L)) / float(len(L)))
'''

f1 = plt.subplot(3, 1, 1)

stats = 'Media:    ' + "{:5d}".format(GAME['pontos'][1]) + '    \n' + \
        'Equil.:   ' + "{:5d}".format(GAME['pontos'][2]) + ' (-)\n' + \
        'Quedas:   ' + "{:5d}".format(GAME['pontos'][3]) + ' (-)\n' + \
        'FINAL:    ' + "{:5d}".format(GAME['pontos'][0]) + '    '
plt.text(0.99, 0.95, stats, va='top', ha='right', transform=f1.transAxes, family='monospace', size=8)

stats = 'Golpes: ' + str(GAME['golpes'])                                 + '\n' + \
        'Ritmo:  ' + str(GAME['ritmo'][0]) + '/' + str(GAME['ritmo'][1]) + '\n' + \
        'Quedas: ' + str(GAME['quedas'])
plt.text(0.01, 0.95, stats, va='top', ha='left', transform=f1.transAxes, family='monospace', size=8)

plt.title(GAME[0]['nome'] + ' / ' + GAME[1]['nome'] + ' / ' +
          str(GAME['distancia']) + 'cm / ' + str(GAME['tempo']) + 's / ' +
          GAME['timestamp'])

plt.xlabel('Velocidade (km/h)')
plt.xlim(xmax=100)
plt.ylabel('Golpes')
plt.ylim(ymax=80)
plt.grid(axis='y')
plt.hist(speeds(GAME[0]['hits']+GAME[1]['hits']), color=['gray'], label='xxx')
plt.hist(speeds(GAME[0]['hits']), color=['blue'], histtype='step')
plt.hist(speeds(GAME[1]['hits']), color=['red'],  histtype='step')
plt.axvline(GAME['ritmo'][0], color='k', linestyle='dashed', linewidth=1)
plt.axvline(GAME['ritmo'][1], color='k', linestyle='dashed', linewidth=1)
#plt.legend()

def atleta (i):
    f = plt.subplot(3, 1, i+2)

    #vol = qmean(speeds(GAME[i]['hits']))
    #esq = np.array(speeds(GAME[i]['left'])).mean()
    #dir = np.array(speeds(GAME[i]['right'])).mean()

    stats = 'Volume:   ' + "{:5d}".format(GAME[i]['pontos'][1]) + '    \n' + \
            'Esquerda: ' + "{:5d}".format(GAME[i]['pontos'][2]) + '    \n' + \
            'Direita:  ' + "{:5d}".format(GAME[i]['pontos'][3]) + '    \n' + \
            'FINAL:    ' + "{:5d}".format(GAME[i]['pontos'][0]) + '    '
    plt.text(1, 0.95, stats, va='top', ha='right', transform=f.transAxes, family='monospace', size=8)

    stats = 'Golpes:   ' + "{:3d}".format(GAME[i]['golpes'])   + '\n' + \
            'Volume:   ' + "{:3d}".format(GAME[i]['ritmo'][0]) + '\n' + \
            'Esquerda: ' + "{:3d}".format(GAME[i]['ritmo'][1]) + '\n' + \
            'Direita:  ' + "{:3d}".format(GAME[i]['ritmo'][2])
    plt.text(0.01, 0.95, stats, va='top', ha='left', transform=f.transAxes, family='monospace', size=8)

    plt.title(GAME[i]['nome'])
    plt.xlabel('Velocidade (km/h)')
    plt.xlim(xmax=100)
    plt.ylabel('Golpes')
    plt.ylim(ymax=40)
    plt.grid(axis='y')
    plt.hist(speeds(GAME[i]['hits']), color=['gray'])
    plt.hist(speeds(GAME[i]['left']), color=['blue'], histtype='step')
    plt.hist(speeds(GAME[i]['right']), color=['red'], histtype='step')
    plt.axvline(GAME[i]['ritmo'][0], color='k', linestyle='dashed', linewidth=1)
    plt.axvline(GAME[i]['ritmo'][1], color='blue', linestyle='dashed', linewidth=1)
    plt.axvline(GAME[i]['ritmo'][2], color='red', linestyle='dashed', linewidth=1)

atleta(0)
atleta(1)

plt.tight_layout(pad=1, w_pad=1, h_pad=1)
plt.savefig('foo.png')
