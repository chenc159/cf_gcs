gnome-terminal --window -e 'bash -c "roslaunch optitrack_broadcast optitrack.launch; exec bash"' \
--tab -e 'bash -c "sleep 2; roslaunch cf_gs connect.launch; exec bash"' \
--tab -e 'bash -c "sleep 2; roslaunch test position_controller.launch uavid:=cf0; exec bash"' \
--tab -e 'bash -c "sleep 2; roslaunch test position_controller.launch uavid:=cf1; exec bash"' \
--tab -e 'bash -c "sleep 2; roslaunch test position_controller.launch uavid:=cf3; exec bash"' \
--tab -e 'bash -c "sleep 2; roslaunch test position_controller.launch uavid:=cf4; exec bash"' \
# --tab -e 'bash -c "sleep 2; roslaunch test position_controller.launch uavid:=cf2; exec bash"' \
# --tab -e 'bash -c "sleep 2; roslaunch test position_controller.launch uavid:=cf5; exec bash"' \


