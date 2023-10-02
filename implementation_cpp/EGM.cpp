#include "shared_functions.h"

RecordIterates EGM(const Params& p)
{
	Iterates iter(p.c.rows(), p.b.rows());
	RecordIterates record(p.c.rows(), p.b.rows(), p.max_iter / p.record_every + 2);
	record.append(iter, p);
	Cache cache;
	cache.z_cur_start = iter.z;
	while (true) {
		EGMStep(iter, p, record);
		AdaptiveRestarts(iter, p, record);
		if (iter.count > p.max_iter) break;
	}

	//std::cout << iter.z << std::endl;
	return record;
}

void EGMStep(Iterates& iter, const Params& p, RecordIterates& record)
{
	iter.z_hat = iter.z - p.eta * compute_F(iter.z, p);
	iter.z_hat.segment(0, p.c.rows()) = iter.z_hat.segment(0, p.c.rows()).cwiseMax(0);
	iter.z = iter.z - p.eta * compute_F(iter.z_hat, p);
	iter.z.segment(0, p.c.rows()) = iter.z.segment(0, p.c.rows()).cwiseMax(0);

	iter.update();

	if ((iter.count - 1) % p.record_every == 0 || (iter.count - 1) % p.print_every == 0)
	{
		iter.compute_convergence_information(p);
		if ((iter.count - 1) % p.record_every == 0)
			record.append(iter, p);
		if ((iter.count - 1) % p.print_every == 0)
			iter.print_iteration_information(p);
	}
}