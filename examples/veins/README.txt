netgenerate --grid --grid.number=3 --grid.length=250 --default.lanenumber=1 --output-file=map.net.xml

Com o comando, criamos um mapa do tipo grid (--grid); com 3 ruas verticais e 3 horizontais (--grid.number=3); cada trecho de rua, como o trecho do ponto 0,0 ao 1,0, possui 250 metros de comprimento (--grid.length=250); cada rua possui 1 via de mão dupla (--default.lanenumber=1); e o nome do arquivo é map.net.xml (--output-file=map.net.xml).

../../../sumo-0.30.0/tools/randomTrips.py --net-file=map.net.xml --min-distance=1000 --begin=0 --end=3600 --route-file=map.rou.xml

O comando acima utilizou a ferramenta random trips para criar rotas para o arquivo map.net.xml (-net-file=map.net.xml); cada rota tem, no mínimo 1000 metros (--min-distance=1000); são criadas 3600 rotas (--begin=0 --end=3600); e tudo isso fica no arquivo de rotas map.rou.xml (--route-file=map.rou.xml).