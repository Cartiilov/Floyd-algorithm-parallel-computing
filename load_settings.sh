source /opt/nfs/config/source_mpich420.sh

touch nodes
chmod 644 nodes

opt/nfs/config/station204_name_list.sh 1 16 > nodes