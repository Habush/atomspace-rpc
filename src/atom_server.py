# Author Abdulrahman S. Omar <xabush@singularitynet.io>
import grpc
from py import atom_server_pb2_grpc
from das.distributed_atom_space import DistributedAtomSpace, QueryOutputFormat
from py.atomspace_service import AtomSpaceService
from py.utils import Timer
from concurrent import futures
import os
import click
import subprocess

@click.group()
def run():
    pass

@run.command()
@click.option("--mongo-host", "-mh", default="localhost", help="MongoDB hostname")
@click.option("--mongo-port", "-mp", default=27017, type=int, help="MongoDB port")
@click.option("--redis-host", "-rh", default="localhost", type=str, help="Redis hostname")
@click.option("--redis-port", "-rp", default=6379, type=int, help="Redis port")
@click.option("--db-username", "-u", type=str, help="Database username")
@click.option("--db-password", "-p", type=str, help="Database password")
@click.option("--hostname", default="localhost", help="Hostname")
@click.option("--port", default=50051, type=int, help="Port")
@click.option("--path", default=".", type=str, help="Path to files containing knowledge base")
@click.option("--load", "-l", is_flag=True, help="Load atoms from files")
def das(mongo_host, mongo_port, redis_host, redis_port,
        db_username, db_password, hostname, port, path=None, load=False):

    os.environ["DAS_REDIS_HOSTNAME"] = redis_host
    os.environ["DAS_REDIS_PORT"] = str(redis_port)
    os.environ["DAS_MONGODB_HOSTNAME"] = mongo_host
    os.environ["DAS_MONGODB_PORT"] = str(mongo_port)
    os.environ["DAS_DATABASE_USERNAME"] = db_username
    os.environ["DAS_DATABASE_PASSWORD"] = db_password

    with Timer("Das Loaded"):
        print(f"Starting DAS...")
        das = DistributedAtomSpace()

        if load:
            if path is not None:
                if os.path.exists(path):
                    print(f"Loading knowledge base from {path}")
                    das.load_knowledge_base(path)
                else:
                    raise ValueError(f"Path to files containing knowledge base doesn't exist")
            else:
                raise ValueError(f"Load option is set to true, however, path to files containing knowledge base is not "
                                 f"provided")


        print(f"Total count: {das.count_atoms()}")

    with Timer("Grpc server started"):
        server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
        atom_server_pb2_grpc.add_AtomServerServicer_to_server(AtomSpaceService(das), server)
        server.add_insecure_port(f'{hostname}:{port}')

    server.start()
    server.wait_for_termination()

@run.command()
@click.option("--config", "-c", type=click.Path(exists=True, readable=True), help="Path to config file")
@click.option("--hostname", "-h", default="localhost", help="Hostname")
@click.option("--port", "-p", default=50051, type=int, help="Port")
def local(config, hostname, port):
    cmd = ["atom_server", "--config", config, "--hostname", hostname, "--port", str(port)]
    subprocess.run(cmd)


if __name__ == '__main__':
    run()