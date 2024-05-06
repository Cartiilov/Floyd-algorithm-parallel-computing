source /opt/nfs/config/source_mpich420.sh
source /opt/nfs/config/source_cuda121.sh

touch ../nodes

chmod 664 ../nodes

/opt/nfs/config/station204_name_list.sh 1 16 > ../nodes